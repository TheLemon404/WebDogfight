#include "time.hpp"

#include <GLFW/glfw3.h>
#include "../utils/math.hpp"

void Clock::Tick() {
    deltaTime = glfwGetTime() - currentTime;
    currentTime = glfwGetTime();

    for (size_t i = 0; i < timers.size(); i++) {
        timers[i].timeElapsed += deltaTime;
        if(timers[i].timeElapsed >= timers[i].endTime) {
            timers[i].callback();
            timers.erase(timers.begin() + i);
        }
    }

    deltaTime = MathUtils::Clamp<float>(deltaTime, 0.0001f, 0.1f);
}
