#pragma once

#include <glfw/glfw3.h>

struct InputMappings {
    inline static unsigned int brake = GLFW_KEY_V;
    inline static unsigned int freeLook = GLFW_KEY_C;
    inline static unsigned int flare = GLFW_KEY_SPACE;
    inline static unsigned int throttleUp = GLFW_KEY_LEFT_SHIFT;
    inline static unsigned int throttleDown = GLFW_KEY_LEFT_CONTROL;
    inline static unsigned int axisYPlus = GLFW_KEY_W;
    inline static unsigned int axisYMinus = GLFW_KEY_S;
    inline static unsigned int axisXPlus = GLFW_KEY_D;
    inline static unsigned int axisXMinus = GLFW_KEY_A;
    inline static unsigned int rollLeft = GLFW_KEY_Q;
    inline static unsigned int rollRight = GLFW_KEY_E;
    inline static unsigned int toggleRadarTarget = GLFW_KEY_T;
};
