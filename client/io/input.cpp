#include "input.hpp"
#include "GLFW/glfw3.h"

void InputManager::GLFWMouseCursorPosCallback(GLFWwindow* window, double x, double y) {
    mouseDelta.x = x - mousePosition.x;
    mouseDelta.y = y - mousePosition.y;
    mousePosition.x = x;
    mousePosition.y = y;
}

void InputManager::GLFWMouseScrollCallback(GLFWwindow* window, double x, double y) {
    mouseScroll.x = x;
    mouseScroll.y = y;
}

void InputManager::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if(button >= GLFW_MAX_MOUSE_BUTTONS){
        std::cout << "Mouse button not properly mapped: " << button << std::endl;
        return;
    }
    lastMouseButtonStates[button] = mouseButtonStates[button];
    mouseButtonStates[button] = action;
}

void InputManager::GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if(key >= GLFW_MAX_KEYS){
        std::cout << "Mouse button not properly mapped: " << key << std::endl;
        return;
    }
    keyStates[key] = action;
}

void InputManager::GLFWCharCallback(GLFWwindow *window, unsigned int codepoint) {
    lastChar = codepoint;
}

void InputManager::ResetInputState() {
    mouseDelta.x = 0.0;
    mouseDelta.y = 0.0;
    mouseScroll.x = 0.0;
    mouseScroll.y = 0.0;

    for(size_t i = 0; i < GLFW_MAX_KEYS; i++) {
        lastKeyStates[i] = keyStates[i];
    }

    for(size_t i = 0; i < GLFW_MAX_MOUSE_BUTTONS; i++) {
        lastMouseButtonStates[i] = mouseButtonStates[i];
    }
}

bool InputManager::IsKeyJustPressed(unsigned int key) {
    if((keyStates[key] == GLFW_PRESS) && lastKeyStates[key] == GLFW_RELEASE) return true;
    return false;
}

bool InputManager::IsKeyPressed(unsigned int key) {
    if(keyStates[key] == GLFW_PRESS || keyStates[key] == GLFW_REPEAT) return true;
    return false;
}

bool InputManager::IsKeyJustReleased(unsigned int key) {
    if(keyStates[key] == GLFW_RELEASE && (lastKeyStates[key] == GLFW_PRESS || lastKeyStates[key] == GLFW_REPEAT)) return true;
    return false;
}

bool InputManager::IsMouseButtonJustPressed(unsigned int button) {
    if(mouseButtonStates[button] == GLFW_PRESS && lastMouseButtonStates[button] == GLFW_RELEASE) return true;
    return false;
}

bool InputManager::IsMouseButtonPressed(unsigned int button) {
    if(mouseButtonStates[button] == GLFW_PRESS || mouseButtonStates[button] == GLFW_REPEAT) return true;
    return false;
}

bool InputManager::IsMouseButtonJustReleased(unsigned int button) {
    if(mouseButtonStates[button] == GLFW_RELEASE && (lastMouseButtonStates[button] == GLFW_PRESS || lastMouseButtonStates[button] == GLFW_REPEAT)) return true;
    return false;
}

char InputManager::GetChar() {
    unsigned int ch = lastChar;
    lastChar = 0;
    return ch;
}
