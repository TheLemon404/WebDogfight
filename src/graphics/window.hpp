#pragma once

#include "backend.hpp"
#include <stdexcept>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window {
    public:
    int width, height;
    const char* title;
    GLFWwindow* window;

    void Open() {
        if(!glfwInit()) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW context");
        }

        window = glfwCreateWindow(width, height, title, NULL, NULL);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


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

    void PollAndSwapBuffers() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    void Close() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};
