#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>

struct Timer {
    float endTime = 0.0f;
    float timeElapsed = 0.0f;
    std::function<void()> callback = []() {};
};

class Clock {
    public:
    double currentTime = 0.0;
    double deltaTime = 0.0;

    std::vector<Timer> timers;

    public:
    void Tick() {
        deltaTime = glfwGetTime() - currentTime;
        currentTime = glfwGetTime();

        for (size_t i = 0; i < timers.size(); i++) {
            timers[i].timeElapsed += deltaTime;
            if(timers[i].timeElapsed >= timers[i].endTime) {
                timers[i].callback();
                timers.erase(timers.begin() + i);
            }
        }
    }
};
