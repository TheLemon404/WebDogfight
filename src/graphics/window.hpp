#pragma once

#include "../io/input.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <stdexcept>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window {
    public:
    int width, height;
    float aspect;
    const char* title;
    GLFWwindow* window;

    Window();

#ifdef __EMSCRIPTEN__
    static EM_BOOL EmscriptenResizeCallback(int event_type, const EmscriptenUiEvent *event, void *user_data);
#else
    static void GLFWResizeCallback(GLFWwindow* window, int width, int height);
#endif

    void Open();

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Poll() {
        glfwPollEvents();

        if(InputManager::IsKeyJustPressed(GLFW_KEY_LEFT_ALT)) {
            InputManager::mouseHidden = !InputManager::mouseHidden;
            glfwSetInputMode(window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
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
    inline static float aspect = 1.0f;
    inline static std::shared_ptr<Window> primaryWindow;

    static glm::mat4 GetUIOrthographicMatrix() {
        aspect = static_cast<float>(primaryWindow->width) / primaryWindow->height;
        return glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.01f, 100.0f);
    }
};
