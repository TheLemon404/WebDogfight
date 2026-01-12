#pragma once

#include "glm/ext/vector_float2.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#define GLFW_MAX_MOUSE_BUTTONS 8
#define GLFW_MAX_KEYS 345

class InputManager {
    inline static int lastMouseButtonStates[GLFW_MAX_MOUSE_BUTTONS];
    inline static int lastKeyStates[GLFW_MAX_KEYS];

    public:
    inline static bool mouseHidden = true;
    inline static glm::vec2 mousePosition = glm::vec2(0.0);
    inline static glm::vec2 mouseDelta = glm::vec2(0.0);
    inline static glm::vec2 mouseScroll = glm::vec2(0.0);

    inline static int mouseButtonStates[GLFW_MAX_MOUSE_BUTTONS];
    inline static int keyStates[GLFW_MAX_KEYS];

    static void GLFWMouseCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
    static void ResetInputState();

    static bool IsKeyJustPressed(unsigned int key);
    static bool IsKeyPressed(unsigned int key);
    static bool IsKeyJustReleased(unsigned int key);

    static bool IsMouseButtonJustPressed(unsigned int button);
    static bool IsMouseButtonPressed(unsigned int button);
    static bool IsMouseButtonJustReleased(unsigned int button);
};
