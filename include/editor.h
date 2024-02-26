#ifndef EDITOR_H
#define EDITOR_H

#define STB_IMAGE_IMPLEMENTATION

#ifdef WIN32
	#include <Windows.h>
#endif

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "frame_event.h"
#include "utils.h"
#include "math_utils.h"
#include "macros.h"

enum class EditorState {
	Normal, NeedToSaved
};

class Editor final {
private:
	GLFWwindow* m_window = nullptr;
	vec2i m_windowSizeOrigin = vec2i(1024, 600);
	bool m_fullscreen = false;
	vec4 m_backgroundColor = vec4(0, 0, 0, 1);
private:
	const vec4 m_normalStateColor = hex2rgba(0x00e000);
	const vec4 m_needToSavedStateColor = hex2rgba(0xe00000);
	EditorState m_state = EditorState::Normal;
public:
	class ColorRect* m_stateVisual = nullptr;
private:
#ifdef BACKGROUND_TEXTURE_PATH
	class Shader* m_backgroundShader = nullptr;
	class Texture* m_backgroundTexture = nullptr;
	unsigned int m_backgroundVAO, m_backgroundVBO, m_backgroundEBO;
#endif
	std::vector<class ColorRect*> m_highlights{};
	class ColorRect* m_cursor{};
	unsigned long long m_cursorPosition = 0;
	unsigned long long m_cursorSelectionPosition = 0;
	unsigned long long m_cursorSelectionEndPosition = 0;
	class Tween* m_posTween = nullptr;
	class Tween* m_zoomTween = nullptr;
	class Label* m_label = nullptr;
	std::string m_filePath = "";
	bool m_waitingForEnter = false;
public:
	FrameEvent currentFrameEvent{};
	vec2i windowSize = m_windowSizeOrigin;
	class Camera* camera = nullptr;
private:
	void updateCursorPos();
	void updateCursorSelectionPos();
	void tryToPushUpCursor();
	void tryToPushUpCursorSelection();
	void tryToPushDownCursor();
	void tryToPushDownCursorSelection();
	void tweenCameraPos(const vec2& amount);
	void tweenCameraZoom(const vec2& amount);
	void tweenCameraZoomOffset(const vec2& amount);
public:
	Editor(const char* _filePath = "");
	~Editor();
	void run();
public:
	void setWindowFullscreen(const bool& flag);
public:
	void charCallback(unsigned int codepoint);
};








#endif