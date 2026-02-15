#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Time {
    public:
    inline static double currentTime = 0.0;
    inline static double deltaTime = 0.0;

    static void Tick() {
        deltaTime = glfwGetTime() - currentTime;
        currentTime = glfwGetTime();
    }
};
