#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Clock {
    public:
    double currentTime = 0.0;
    double deltaTime = 0.0;

    void Tick() {
        deltaTime = glfwGetTime() - currentTime;
        currentTime = glfwGetTime();
    }
};
