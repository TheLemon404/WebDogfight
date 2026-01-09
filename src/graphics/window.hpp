#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

#include "../io/input.hpp"
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#include <stdexcept>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Window {
    public:
    int width, height;
    const char* title;
    GLFWwindow* window;

#ifdef __EMSCRIPTEN__
    static EM_BOOL EmscriptenResizeCallback(int event_type, const EmscriptenUiEvent *event, void *user_data) {
        Window* window = static_cast<Window*>(user_data);
        if(window){
            window->width = event->windowInnerWidth;
            window->height = event->windowInnerHeight;
            return 1;
        }
        return 0;
    }
#else
    static void GLFWResizeCallback(GLFWwindow* window, int width, int height) {
        Window* userWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if(userWindow){
            userWindow->width = width;
            userWindow->height = height;
        }
        else {
            throw std::runtime_error("Failed to get user pointer from window");
        }
    }
#endif

    void Open() {
        if(!glfwInit()) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW context");
        }

        window = glfwCreateWindow(width, height, title, NULL, NULL);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwSetWindowUserPointer(window, this);

#ifdef __EMSCRIPTEN__
        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, Window::EmscriptenResizeCallback);
#else
        glfwSetFramebufferSizeCallback(window, Window::GLFWResizeCallback);
#endif

        glfwSetScrollCallback(window, InputManager::GLFWMouseScrollCallback);
        glfwSetCursorPosCallback(window, InputManager::GLFWMouseCursorPosCallback);
        glfwSetMouseButtonCallback(window, InputManager::GLFWMouseButtonCallback);
        glfwSetKeyCallback(window, InputManager::GLFWKeyCallback);

        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
        gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);
#endif
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Poll() {
        glfwPollEvents();
    }

    void SwapBuffers() {
        glfwSwapBuffers(window);
    }

    void Close() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

class WindowManager {
    public:
    inline static std::shared_ptr<Window> primaryWindow;

    static glm::mat4 GetUIOrthographicMatrix() {
        float widthFraction = static_cast<float>(primaryWindow->width) / primaryWindow->height;
        return glm::ortho(-widthFraction, widthFraction, -1.0f, 1.0f, 0.01f, 100.0f);
    }
};
