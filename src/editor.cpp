#include "editor.h"
#include "macros.h"
#include "tween.h"
#include "label.h"
#include "camera.h"
#include "color_rect.h"
#include "glyph_texture.h"
#include "shader.h"
#include <cassert>
#include <utility>

extern Editor* myEditor;

static void bufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    myEditor->windowSize.x = width;
    myEditor->windowSize.y = height;
    myEditor->camera->setWindowSize(width, height);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    myEditor->currentFrameEvent.justKeys[key] = { scancode, action, mods };
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    myEditor->currentFrameEvent.mouse.scroll.xOffset = xoffset;
    myEditor->currentFrameEvent.mouse.scroll.yOffset = yoffset;
}

static void mousePosCallback(GLFWwindow* window, double xposIn, double yposIn) {
    myEditor->currentFrameEvent.mouse.pos.xPos = xposIn;
    myEditor->currentFrameEvent.mouse.pos.yPos = yposIn;
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    myEditor->currentFrameEvent.mouse.buttons[button] = { action, mods };
}

static void windowCloseCallback(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void setCharCallback(GLFWwindow* window, unsigned int codepoint) {
    myEditor->charCallback(codepoint);
}


Editor::Editor(const char* _filePath) : m_filePath(_filePath) {
    if (!glfwInit()) {
        return;
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    m_window = glfwCreateWindow(windowSize.x, windowSize.y, "Editor", NULL, NULL);
    if (!m_window) {
        PUSH_ERROR("Failed To Make New GLFW Window");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        PUSH_ERROR("Failed To Load GL Functions");
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return;
    }
    const GLFWvidmode* _videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_window, (_videoMode->width / 2) - (windowSize.x / 2), (_videoMode->height / 2) - (windowSize.y / 2));
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(m_window, bufferSizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);
    glfwSetCursorPosCallback(m_window, mousePosCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetWindowCloseCallback(m_window, windowCloseCallback);
    glfwSetCharCallback(m_window, setCharCallback);
    currentFrameEvent.clear();
    camera = new Camera(windowSize.x, windowSize.y);
    camera->setPosition(vec2(-(windowSize.x / 4), (windowSize.y / 4)));
    m_label = new Label(camera, (m_filePath != "" ? readFileW(s2ws(m_filePath)) : L""));
    m_posTween = new Tween();
    m_zoomTween = new Tween();
    m_cursor = new ColorRect(camera);
    m_cursor->setSize(vec2i(2, FONT_SIZE));
    m_cursorPosition = 0;
    if (m_filePath != "") {
        m_stateVisual = new ColorRect(camera);
        m_stateVisual->setIgnoreViewMatrix(true);
        m_stateVisual->setPosition(vec2(0.0f, m_windowSizeOrigin.y / 2 - 5));
        m_stateVisual->setSize(vec2i(m_windowSizeOrigin.x, 10));
        m_stateVisual->setColor(m_normalStateColor);
    }
    updateCursorPos();
#ifdef BACKGROUND_TEXTURE_PATH
    m_backgroundShader = new Shader(readFile(SPRITE_VERTEX_SHADER_PATH).c_str(), readFile(SPRITE_FRAGMENT_SHADER_PATH).c_str());
    int width, height, channels;
    unsigned char* data = stbi_load(BACKGROUND_TEXTURE_PATH, &width, &height, &channels, 0);
    if (data == nullptr) {
        PUSH_ERROR("Cannot Load Image File");
        return;
    }
    m_backgroundTexture = new Texture(data, width, height, channels);
    stbi_image_free(data);

    float vertices[] = {
        -0.5f,  0.5f, 0.f, 0.f, 0.f,
        0.5f,  0.5f, 0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.f, 1.f, 1.f,
        -0.5f, -0.5f, 0.f, 0.f, 1.f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    glGenVertexArrays(1, &m_backgroundVAO);
    glBindVertexArray(m_backgroundVAO);
    glGenBuffers(1, &m_backgroundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &m_backgroundEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_backgroundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 3));
#endif
}

Editor::~Editor() {
#ifdef BACKGROUND_TEXTURE_PATH
    glDeleteBuffers(1, &m_backgroundVBO);
    glDeleteBuffers(1, &m_backgroundEBO);
    glDeleteVertexArrays(1, &m_backgroundVAO);
    delete m_backgroundTexture;
#endif
    delete m_label, camera, m_posTween, m_zoomTween;
    if (m_stateVisual != nullptr) {
        delete m_stateVisual;
    }
}

void Editor::run() {
    bool backspaceStarted = false;
    bool enterStarted = false;
    int backspaceCount = 0;
    int enterCount = 0;
    const int MAX_COUNT = 32;
    enum class CursorPos{
        Left, Right, Up, Down
    };
    bool cursorStarted[4] = { false, false, false, false };
    int cursorCount[4] = { 0, 0, 0, 0 };
    int cursorFlashCount = 0;
    int KEYS[4] = { GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN };
    bool CONDITIONS[4] = {false, false, false, false};
    while (!glfwWindowShouldClose(m_window)) {
        for (int i = 0; i < 349; i++) { currentFrameEvent.pressedKeys[i] = glfwGetKey(m_window, i); }
        if (currentFrameEvent.justKeys[GLFW_KEY_F11].action == GLFW_PRESS) {
            setWindowFullscreen(!m_fullscreen);
        }
#pragma region lazy_key_input
        CONDITIONS[(int)CursorPos::Right] = m_label->getGlyphTextures().size() >= m_cursorPosition + 1;
        CONDITIONS[(int)CursorPos::Left] = 0 != m_cursorPosition;
        CONDITIONS[(int)CursorPos::Up] = true;
        CONDITIONS[(int)CursorPos::Down] = true;

        for (int i = 0; i < 4; i++) {
            int CUR_KEY = KEYS[i];
            if (currentFrameEvent.justKeys[CUR_KEY].action == GLFW_PRESS) {
                cursorFlashCount = 0;
                m_waitingForEnter = false;
                m_cursor->setVisible(true);
                if (cursorStarted[i] == false) {
                    if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_SHIFT] ||
                        currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_SHIFT]
                        ) {
                            switch (CursorPos(i)) {
                            case (CursorPos::Right):
                                m_cursorSelectionPosition++;
                                break;
                            case (CursorPos::Left):
                                if (m_cursorSelectionPosition != 0) {
                                    m_cursorSelectionPosition--;
                                }
                                break;
                            case (CursorPos::Up):
                                tryToPushUpCursorSelection();
                                break;
                            case (CursorPos::Down):
                                tryToPushDownCursorSelection();
                                break;
                            }
                            updateCursorSelectionPos();
                    }
                    else {
                    if (CONDITIONS[i]) {
                        switch (CursorPos(i)) {
                        case (CursorPos::Right):
                            m_cursorPosition++;
                            break;
                        case (CursorPos::Left):
                            m_cursorPosition--;
                            break;
                        case (CursorPos::Up):
                            tryToPushUpCursor();
                            break;
                        case (CursorPos::Down):
                            tryToPushDownCursor();
                            break;
                        }
                        m_cursorSelectionPosition = m_cursorPosition;
                        m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                        updateCursorSelectionPos();
                        updateCursorPos();
                    }
                    }
                    cursorStarted[i] = true;
                }
            }
            if (currentFrameEvent.pressedKeys[CUR_KEY] == GLFW_RELEASE) {
                if (cursorStarted[i]) {
                    cursorStarted[i] = false;
                    cursorCount[i] = 0;
                }
            }
            if (currentFrameEvent.pressedKeys[CUR_KEY] == GLFW_PRESS) {
                m_waitingForEnter = false;
                if (cursorCount[i] == MAX_COUNT) {
                    if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_SHIFT] ||
                        currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_SHIFT]
                        ) {
                            switch (CursorPos(i)) {
                            case (CursorPos::Right): {
                                m_cursorSelectionPosition++;
                                break;
                            }
                            case (CursorPos::Left):
                                if (m_cursorSelectionPosition != 0) {
                                    m_cursorSelectionPosition--;
                                }
                                break;
                            case (CursorPos::Up):
                                tryToPushUpCursorSelection();
                                break;
                            case (CursorPos::Down):
                                tryToPushDownCursorSelection();
                                break;
                            }
                            updateCursorSelectionPos();
                    }
                    else {
                    if (CONDITIONS[i]) {
                        switch (CursorPos(i)) {
                        case (CursorPos::Right):
                            m_cursorPosition++;
                            break;
                        case (CursorPos::Left):
                            m_cursorPosition--;
                            break;
                        case (CursorPos::Up):
                            tryToPushUpCursor();
                            break;
                        case (CursorPos::Down):
                            tryToPushDownCursor();
                            break;
                        }
                        m_cursorSelectionPosition = m_cursorPosition;
                        m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                        updateCursorSelectionPos();
                        updateCursorPos();
                    }
                    }
                }
                else {
                    if (cursorCount[i] + 1 <= MAX_COUNT) {
                        cursorCount[i]++;
                    }
                }
            }
        }
#pragma endregion
        if (currentFrameEvent.justKeys[GLFW_KEY_BACKSPACE].action == GLFW_PRESS) {
            if (backspaceStarted == false) {
                if (m_cursorSelectionPosition != m_cursorSelectionEndPosition) {
                    size_t targetCount = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionEndPosition - m_cursorSelectionPosition : m_cursorSelectionPosition - m_cursorSelectionEndPosition);
                    size_t start = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionPosition : m_cursorSelectionEndPosition);
                    for (size_t i = 0; i < targetCount; i++) {
                        m_label->erase(start);
                    }
                    m_cursorPosition = start;
                    m_cursorSelectionPosition = m_cursorPosition;
                    m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                    updateCursorPos();
                    updateCursorSelectionPos();
                }
                else {
                    if (m_cursorPosition > 0 && m_cursorPosition <= m_label->getGlyphTextures().size()) {
                        if (m_filePath != "") {
                            m_state = EditorState::NeedToSaved;
                            m_stateVisual->setColor(m_needToSavedStateColor);
                        }
                        int blockSizeYBefore = m_label->getBlockList().size();
                        m_cursorPosition--;
                        m_cursorSelectionPosition = m_cursorPosition;
                        m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                        if (m_label->getBelongBlock(m_cursorPosition) == m_label->getLongestBlock()) {
                            camera->addZoom(vec2(0.01f));
                        }
                        m_label->erase(m_cursorPosition);
                        updateCursorPos();
                        updateCursorSelectionPos();
                        int blockSizeYAfter = m_label->getBlockList().size();
                        if (blockSizeYBefore != blockSizeYAfter) {
                            m_label->eraseSelectionSection(m_label->getBelongBlock(m_cursorPosition));
                        }
                    }
                }
                backspaceStarted = true;
            }
        }
        if (currentFrameEvent.pressedKeys[GLFW_KEY_BACKSPACE] == GLFW_RELEASE) {
            if (backspaceStarted) {
                backspaceStarted = false;
                backspaceCount = 0;
            }
        }
        if (currentFrameEvent.pressedKeys[GLFW_KEY_BACKSPACE] == GLFW_PRESS && backspaceStarted) {
            if (backspaceCount == MAX_COUNT) {
                if (m_cursorSelectionPosition != m_cursorSelectionEndPosition) {
                    size_t targetCount = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionEndPosition - m_cursorSelectionPosition : m_cursorSelectionPosition - m_cursorSelectionEndPosition);
                    size_t start = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionPosition : m_cursorSelectionEndPosition);
                    for (size_t i = 0; i < targetCount; i++) {
                        m_label->erase(start);
                    }
                    m_cursorPosition = start;
                    m_cursorSelectionPosition = m_cursorPosition;
                    m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                    updateCursorPos();
                    updateCursorSelectionPos();
                }
                else {
                    if (m_cursorPosition > 0 && m_cursorPosition <= m_label->getGlyphTextures().size()) {
                        if (m_filePath != "") {
                            m_state = EditorState::NeedToSaved;
                            m_stateVisual->setColor(m_needToSavedStateColor);
                        }
                        int blockSizeYBefore = m_label->getBlockList().size();
                        m_cursorPosition--;
                        m_cursorSelectionPosition = m_cursorPosition;
                        m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                        if (m_label->getBelongBlock(m_cursorPosition) == m_label->getLongestBlock()) {
                            camera->addZoom(vec2(0.01f));
                        }
                        m_label->erase(m_cursorPosition);
                        updateCursorPos();
                        updateCursorSelectionPos();
                        int blockSizeYAfter = m_label->getBlockList().size();
                        if (blockSizeYBefore != blockSizeYAfter) {
                            m_label->eraseSelectionSection(m_label->getBelongBlock(m_cursorPosition));
                        }
                    }
                }
            }
            else {
                if (backspaceCount + 1 <= MAX_COUNT) {
                    backspaceCount++;
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_ENTER].action == GLFW_PRESS) {
            if (enterStarted == false) {
                if (m_filePath != "") {
                    m_state = EditorState::NeedToSaved;
                    m_stateVisual->setColor(m_needToSavedStateColor);
                }
                float before = m_label->getSize().y;
                if (m_waitingForEnter) {
                    m_waitingForEnter = false;
                    m_label->insert(m_cursorPosition, L'\n');
                    m_label->addSelectionSection();
                }
                m_label->insert(m_cursorPosition, L'\n');
                if (m_cursorPosition + 1 <= m_label->getGlyphTextures().size()) {
                    m_cursorPosition++;
                }
                m_cursorSelectionPosition = m_cursorPosition;
                m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                float after = m_label->getSize().y;
                updateCursorPos();
                updateCursorSelectionPos();
                m_label->addSelectionSection();
                enterStarted = true;
            }
        }
        if (currentFrameEvent.pressedKeys[GLFW_KEY_ENTER] == GLFW_RELEASE) {
            if (enterStarted) {
                enterStarted = false;
                enterCount = 0;
            }
        }
        if (currentFrameEvent.pressedKeys[GLFW_KEY_ENTER] == GLFW_PRESS && enterStarted) {
            if (enterCount == MAX_COUNT) {
                if (m_filePath != "") {
                    m_state = EditorState::NeedToSaved;
                    m_stateVisual->setColor(m_needToSavedStateColor);
                }
                float before = m_label->getSize().y;
                if (m_waitingForEnter) {
                    m_waitingForEnter = false;
                    m_label->insert(m_cursorPosition, L'\n');
                    m_label->addSelectionSection();
                }
                m_label->insert(m_cursorPosition, L'\n');
                if (m_cursorPosition + 1 <= m_label->getGlyphTextures().size()) {
                    m_cursorPosition++;
                }
                m_cursorSelectionPosition = m_cursorPosition;
                m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                float after = m_label->getSize().y;
                updateCursorPos();
                updateCursorSelectionPos();
                m_label->addSelectionSection();
            }
            else {
                if (enterCount + 1 <= MAX_COUNT) {
                    enterCount++;
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_TAB].action == GLFW_PRESS) {
            if (m_filePath != "") {
                m_state = EditorState::NeedToSaved;
                m_stateVisual->setColor(m_needToSavedStateColor);
            }
            for (int i = 0; i < TAB_SIZE; i++) {
                m_label->insert(m_cursorPosition, L' ');
                if (m_cursorPosition + 1 <= m_label->getGlyphTextures().size()) {
                    m_cursorPosition++;
                    m_cursorSelectionPosition++;
                }
            }
            updateCursorPos();
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_MINUS].action == GLFW_PRESS || currentFrameEvent.justKeys[GLFW_KEY_EQUAL].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] ||
                currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]
                ) {
                if (currentFrameEvent.justKeys[GLFW_KEY_MINUS].action == GLFW_PRESS) {
                    tweenCameraZoomOffset(vec2(-0.1));
                }
                else {
                    tweenCameraZoomOffset(vec2(0.1));
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_S].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_state == EditorState::NeedToSaved) {
                    if (m_filePath != "") {
                        writeFile(s2ws(m_filePath), m_label->getText());
                        m_state = EditorState::Normal;
                        m_stateVisual->setColor(m_normalStateColor);
                    }
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_X].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_cursorSelectionPosition != m_cursorSelectionEndPosition) {
                    if (m_filePath != "") {
                        m_state = EditorState::NeedToSaved;
                        m_stateVisual->setColor(m_needToSavedStateColor);
                    }
                    size_t start = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionPosition : m_cursorSelectionEndPosition);
                    size_t end = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionEndPosition : m_cursorSelectionPosition);
                    for (size_t i = start; i < end ; i++) {
                        m_label->erase(start);
                        if (m_cursorPosition != 0 && m_cursorPosition != start) {
                            m_cursorPosition--;
                        }
                    }
                    m_cursorPosition = std::clamp((size_t)m_cursorPosition, size_t(0), m_label->getText().size());
                    m_cursorSelectionPosition = std::clamp((size_t)m_cursorSelectionPosition, size_t(0), m_label->getText().size());
                    m_cursorSelectionEndPosition = std::clamp((size_t)m_cursorSelectionEndPosition, size_t(0), m_label->getText().size());
                    updateCursorPos();
                    updateCursorSelectionPos();
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_C].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_cursorSelectionPosition != m_cursorSelectionEndPosition) {
                    std::wstring selection = L"";
                    size_t start = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionPosition : m_cursorSelectionEndPosition);
                    size_t end = (m_cursorSelectionPosition < m_cursorSelectionEndPosition ? m_cursorSelectionEndPosition : m_cursorSelectionPosition);
                    for (size_t i = start; i < end; i++) {
                        selection += m_label->getText()[i];
                    }
                    glfwSetClipboardString(m_window, multibyte2utf8(selection).c_str());
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_V].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_filePath != "") {
                    m_state = EditorState::NeedToSaved;
                    m_stateVisual->setColor(m_needToSavedStateColor);
                }
                std::string last = glfwGetClipboardString(m_window);
                last = replaceAll(last, "\r", "");
                for (size_t i = 0; i < last.size(); i++) {
                    m_label->insert(m_cursorPosition + i, last[i]);
                }
                m_cursorPosition += last.size();
                m_cursorSelectionPosition = m_cursorPosition;
                m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                updateCursorPos();
                updateCursorSelectionPos();
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_A].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_label->getText().size() > 0) {
                    m_cursorPosition = m_label->getText().size();
                    m_cursorSelectionPosition = 0;
                    m_cursorSelectionEndPosition = m_cursorPosition;
                    updateCursorPos();
                    updateCursorSelectionPos();
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_D].action == GLFW_PRESS) {
            if (currentFrameEvent.pressedKeys[GLFW_KEY_LEFT_CONTROL] || currentFrameEvent.pressedKeys[GLFW_KEY_RIGHT_CONTROL]) {
                if (m_label->getText().size() > 0) {
                    if (m_filePath != "") {
                        m_state = EditorState::NeedToSaved;
                        m_stateVisual->setColor(m_needToSavedStateColor);
                    }
                    std::wstring curLine = L"";
                    size_t curLineStart = m_label->getBlockList()[m_label->getBelongBlock(m_cursorPosition)].first;
                    size_t curLineEnd = m_label->getBlockList()[m_label->getBelongBlock(m_cursorPosition)].second;
                    for (size_t i = curLineStart; i < curLineEnd; i++) {
                        curLine += m_label->getText()[i];
                    }
                    m_label->insert(curLineEnd, L'\n');
                    m_label->addSelectionSection();
                    curLineEnd++;
                    m_cursorPosition++;
                    if (curLineStart + 1 < curLineEnd) {
                        for (size_t i = curLine.size() - 1; i > 0; i--) {
                            m_label->insert(curLineEnd, curLine[i]);
                            m_cursorPosition++;
                        }
                        m_label->insert(curLineEnd, curLine[0]);
                        m_cursorPosition++;
                    }
                    m_cursorSelectionPosition = m_cursorPosition;
                    m_cursorSelectionEndPosition = m_cursorSelectionPosition;
                    updateCursorPos();
                    updateCursorSelectionPos();
                }
            }
        }
        if (currentFrameEvent.justKeys[GLFW_KEY_F1].action == GLFW_PRESS) {
            m_label->toggleRainbow();
        }
        if (0 <= cursorFlashCount && cursorFlashCount < 30) {
            m_cursor->setVisible(true);
        }
        else if (30 <= cursorFlashCount && cursorFlashCount < 60) {
            if (!backspaceStarted &&
                !cursorStarted[0] &&
                !cursorStarted[1] &&
                !cursorStarted[2] &&
                !cursorStarted[3]
                ) {
                m_cursor->setVisible(false);
            }
        }
        else {
            cursorFlashCount = 0;
        }
        m_zoomTween->update();
        m_posTween->update();
        m_label->update();
        cursorFlashCount++;
        glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef BACKGROUND_TEXTURE_PATH
        {
            glBindVertexArray(m_backgroundVAO);
            m_backgroundShader->use();
            m_backgroundTexture->bind();
            m_backgroundShader->setMat4("ProjectionMatrix", camera->getProjectionMatrix().data());
            m_backgroundShader->setVec4("Modulate", vec4(BACKGROUND_TEXTURE_MODULATE_RGB, BACKGROUND_TEXTURE_MODULATE_RGB, BACKGROUND_TEXTURE_MODULATE_RGB, 1.0f));
            m_backgroundShader->setVec2("TexSize", vec2(windowSize.x, windowSize.y));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            m_backgroundShader->unuse();
            glBindVertexArray(0);
        }
#endif
        m_label->draw();
        for (const auto& cl : m_highlights) {
            cl->draw();
        }
        m_cursor->draw();
        if (m_stateVisual != nullptr) {
            m_stateVisual->draw();
        }
        currentFrameEvent.clear();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Editor::charCallback(unsigned int codepoint) {
    if (m_filePath != "") {
        m_state = EditorState::NeedToSaved;
        m_stateVisual->setColor(m_needToSavedStateColor);
    }
    if (wchar_t(codepoint) == L'{') {
        m_waitingForEnter = true;
        m_label->insert(m_cursorPosition, codepoint);
        if (m_cursorPosition + 1 <= m_label->getGlyphTextures().size()) {
            m_cursorPosition++;
            m_cursorSelectionPosition = m_cursorPosition;
            m_cursorSelectionEndPosition = m_cursorSelectionPosition;
        }
        m_label->insert(m_cursorPosition, L'}');
        updateCursorPos();
        return;
    }
    float before = m_label->getSize().x;
    m_label->insert(m_cursorPosition, codepoint);
    float after = m_label->getSize().x;
    if (m_cursorPosition + 1 <= m_label->getGlyphTextures().size()) {
        m_cursorPosition++;
        m_cursorSelectionPosition = m_cursorPosition;
        m_cursorSelectionEndPosition = m_cursorSelectionPosition;
        camera->addZoom(vec2(-0.01f));
    }
    updateCursorPos();
    updateCursorSelectionPos();
}

void Editor::setWindowFullscreen(const bool& flag) {
    m_fullscreen = flag;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (flag) {
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        windowSize.x = mode->width;
        windowSize.y = mode->height;
    }
    else {
        int originW = m_windowSizeOrigin.x;
        int originH = m_windowSizeOrigin.y;
        glfwSetWindowMonitor(m_window, nullptr, (mode->width / 2) - (originW / 2), (mode->height / 2) - (originH / 2), originW, originH, GLFW_DONT_CARE);
        windowSize.x = originW;
        windowSize.y = originH;
    }
    glfwSwapInterval(1);
    glViewport(0, 0, windowSize.x, windowSize.y);
    myEditor->camera->setWindowSize(windowSize.x, windowSize.y);
    if (m_stateVisual != nullptr) {
        m_stateVisual->setPosition(vec2(0.0f, windowSize.y / 2 - 5));
        m_stateVisual->setSize(vec2i(windowSize.x, 10));
    }
}

void Editor::updateCursorPos() {
    vec2 pos = vec2();
    if (m_cursorPosition == 0) {
        pos.x = 0.0f;
        pos.y = 10;
    }
    else {
        int escapeSequenceCount = 0;
        for (unsigned long long i = 0; i < m_cursorPosition; i++) {
            if (m_label->getGlyphTextures()[i].has_value() && m_label->getGlyphTextures()[i].value() != nullptr) {
                pos.x += (m_label->getGlyphTextures()[i].value()->getAdvanceX() >> 6);
            }
            else {
                escapeSequenceCount++;
                pos.x = 0.0f;
                pos.y = 10 + FONT_SIZE * escapeSequenceCount;
            }
        }
        if (pos.y == 0.0f) {
            pos.y = 10;
        }
    }
    vec2 factor = vec2(m_cursor->getPosition()) - pos;
    m_cursor->setPosition(pos);
    camera->addPosition(vec2(factor.x, -factor.y));
}

void Editor::updateCursorSelectionPos() {
    m_cursorSelectionPosition = std::clamp(m_cursorSelectionPosition, 0ULL, (unsigned long long)m_label->getText().size());
    m_cursorSelectionEndPosition = m_cursorPosition;
    if (m_cursorPosition == m_cursorSelectionPosition) {
        m_label->updateSelection(m_cursorPosition, m_cursorPosition);
    }
    else if (m_cursorPosition < m_cursorSelectionPosition) {
        m_label->updateSelection(m_cursorPosition, m_cursorSelectionPosition);
    }
    else if (m_cursorSelectionPosition < m_cursorPosition) {
        m_label->updateSelection(m_cursorSelectionPosition, m_cursorPosition);
    }
}

void Editor::tryToPushUpCursor(){
    int belongBlockIndex = m_label->getBelongBlock(m_cursorPosition);
    if (belongBlockIndex <= 0) {
        return;
    }
    int targetBlockIndex = belongBlockIndex - 1;
    const std::vector<std::pair<int, int>>& blockList = m_label->getBlockList();
    const std::pair<int, int>& belongBlock = blockList[belongBlockIndex];
    const std::pair<int, int>& targetBlock = blockList[targetBlockIndex];
    const int currentLength = m_cursorPosition - belongBlock.first;
    m_cursorPosition = clamp(targetBlock.first + currentLength, targetBlock.first, targetBlock.second);
}

void Editor::tryToPushDownCursor() {
    int belongBlockIndex = m_label->getBelongBlock(m_cursorPosition);
    if (belongBlockIndex >= m_label->getBlockList().size() + 1) {
        return;
    }
    if (belongBlockIndex == -1) {
        belongBlockIndex = 0;
    }
    int targetBlockIndex = belongBlockIndex + 1;
    if (targetBlockIndex >= m_label->getBlockList().size()) {
        return;
    }
    const std::vector<std::pair<int, int>>& blockList = m_label->getBlockList();
    const std::pair<int, int>& belongBlock = blockList[belongBlockIndex];
    const std::pair<int, int>& targetBlock = blockList[targetBlockIndex];
    const int currentLength = m_cursorPosition - belongBlock.first;
    m_cursorPosition = clamp(targetBlock.first + currentLength, targetBlock.first, targetBlock.second);
}

void Editor::tryToPushUpCursorSelection() {
    int belongBlockIndex = m_label->getBelongBlock(m_cursorSelectionPosition);
    if (belongBlockIndex <= 0) {
        return;
    }
    int targetBlockIndex = belongBlockIndex - 1;
    const std::vector<std::pair<int, int>>& blockList = m_label->getBlockList();
    const std::pair<int, int>& belongBlock = blockList[belongBlockIndex];
    const std::pair<int, int>& targetBlock = blockList[targetBlockIndex];
    const int currentLength = m_cursorSelectionPosition - belongBlock.first;
    m_cursorSelectionPosition = clamp(targetBlock.first + currentLength, targetBlock.first, targetBlock.second);
}

void Editor::tryToPushDownCursorSelection() {
    int belongBlockIndex = m_label->getBelongBlock(m_cursorSelectionPosition);
    if (belongBlockIndex >= m_label->getBlockList().size() + 1) {
        return;
    }
    if (belongBlockIndex == -1) {
        belongBlockIndex = 0;
    }
    int targetBlockIndex = belongBlockIndex + 1;
    if (targetBlockIndex >= m_label->getBlockList().size()) {
        return;
    }
    const std::vector<std::pair<int, int>>& blockList = m_label->getBlockList();
    const std::pair<int, int>& belongBlock = blockList[belongBlockIndex];
    const std::pair<int, int>& targetBlock = blockList[targetBlockIndex];
    const int currentLength = m_cursorSelectionPosition - belongBlock.first;
    m_cursorSelectionPosition = clamp(targetBlock.first + currentLength, targetBlock.first, targetBlock.second);
}

void Editor::tweenCameraPos(const vec2& amount) {
    TweenInfo info; {
        info.Object = camera;
        info.StartValue = camera->getPosition();
        info.EndValue = info.StartValue - amount;
        info.PropertyType = TweenablePropertyType::CameraPosition;
        info.TransitionType = TweenTransitionType::TRANS_SINE;
        info.Duration = 0.3f;
    }
    m_posTween->interpolateProperty(info);
    m_posTween->start();
}

void Editor::tweenCameraZoom(const vec2& amount) {
    if (m_zoomTween->getIsEmpty() == false) return;
    TweenInfo info; {
        info.Object = camera;
        info.StartValue = camera->getZoom();
        info.EndValue = info.StartValue + amount;
        info.PropertyType = TweenablePropertyType::CameraZoom;
        info.TransitionType = TweenTransitionType::TRANS_SINE;
        info.Duration = 0.3f;
    }
    m_zoomTween->interpolateProperty(info);
    m_zoomTween->start();
}

void Editor::tweenCameraZoomOffset(const vec2& amount) {
    if (m_zoomTween->getIsEmpty() == false) return;
    TweenInfo info; {
        info.Object = camera;
        info.StartValue = camera->getZoomOffset();
        info.EndValue = info.StartValue + amount;
        info.PropertyType = TweenablePropertyType::CameraZoomOffset;
        info.TransitionType = TweenTransitionType::TRANS_SINE;
        info.Duration = 0.3f;
    }
    m_zoomTween->interpolateProperty(info);
    m_zoomTween->start();
}