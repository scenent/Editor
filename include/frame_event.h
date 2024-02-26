#ifndef FRAME_EVENT_H
#define FRAME_EVENT_H


struct KeyInfo {
    int scancode, action, mods;
};
struct MouseInfo {
    struct MousePosition { double xPos, yPos; };
    struct MouseButton { int action, mods; };
    struct MouseScroll { double xOffset, yOffset; };
    struct MouseRelative { double xRel, yRel; };
    MousePosition pos;
    MousePosition lastPos;
    MouseButton buttons[3];
    MouseButton lastButtons[3];
    MouseScroll scroll;
    MouseRelative rel;
    bool firstIn = true;
};
struct WindowInfo {
    struct WindowSize { int width, height; };
    WindowSize size;
};

struct FrameEvent {
    KeyInfo justKeys[349];
    bool pressedKeys[349];
    MouseInfo mouse;
    WindowInfo window;
    void clear() {
        for (int i = 0; i < 3; i++) mouse.buttons[i] = { -1, 0 };
        for (int i = 0; i < 3; i++) mouse.lastButtons[i] = { -1, 0 };
        for (int i = 0; i < 349; i++) justKeys[i] = { 0, 0, 0 };
        for (int i = 0; i < 349; i++) pressedKeys[i] = false;
        mouse.scroll = {};
    }
};




#endif