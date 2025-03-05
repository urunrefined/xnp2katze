#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Input.h"
#include "util/Rect.h"

namespace BR {

class GLFWInput : public Input {
  public:
    GLFWwindow *window;
    bool shouldClose;

    void pollEvents() {
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            shouldClose = true;
        }
    };

    std::vector<uint32_t> codepoints;

    void reset() {
        codepoints.clear();
        Input::reset();
    }

    GLFWInput() : window(nullptr), shouldClose(false) {}
    virtual ~GLFWInput() {}
};

enum class WindowState { SHOULDCLOSE = 0, MINIMIZED, FOCUSED };

class GLFWContext {
  public:
    uint32_t currentWidth;
    uint32_t currentHeight;

    uint32_t xOffset;
    uint32_t yOffset;

    uint32_t fWidth;
    uint32_t fheight;

    uint32_t requestedWindowWidth;
    uint32_t requestedWindowHeight;

    bool forcePresent;

    GLFWwindow *window;

    GLFWContext(uint32_t width_, uint32_t height_);
    ~GLFWContext();

    GLFWInput input;

    Dimensions2D getCurrentSize();

    GLFWInput &getInput();

    void wait(double timeout);
    void wait();

    void pollWindowEvents() { glfwPollEvents(); }

    WindowState getWindowState() {
        if (glfwWindowShouldClose(window)) {
            return WindowState::SHOULDCLOSE;
        }
        return WindowState::FOCUSED;
    }
};

std::vector<const char *> getRequiredGLFWExtensions();

} // namespace BR
