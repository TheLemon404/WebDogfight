#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#define GLFW_MAX_MOUSE_BUTTONS 8
#define GLFW_MAX_KEYS 256

class InputManager {
    public:
    inline static double mousePositionX = 0.0;
    inline static double mousePositionY = 0.0;
    inline static double mouseDeltaX = 0.0;
    inline static double mouseDeltaY = 0.0;
    inline static int mouseButtonStates[GLFW_MAX_MOUSE_BUTTONS];
    inline static int keyStates[GLFW_MAX_KEYS];

    static void GLFWMouseCursorPosCallback(GLFWwindow* window, double x, double y) {
        mouseDeltaX = x - mousePositionX;
        mouseDeltaY = y - mousePositionY;
        mousePositionX = x;
        mousePositionY = y;
    }

    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if(button >= GLFW_MAX_MOUSE_BUTTONS){
            std::cout << "Mouse button not properly mapped: " << button << std::endl;
            return;
        }
        mouseButtonStates[button] = action;
    }

    static void GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
        if(key >= GLFW_MAX_KEYS){
            std::cout << "Mouse button not properly mapped: " << key << std::endl;
            return;
        }
        keyStates[key] = action;
    }

    static void ResetInputState() {
        mouseDeltaX = 0.0f;
        mouseDeltaY = 0.0f;
    }

};
