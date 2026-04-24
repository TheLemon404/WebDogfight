#pragma once

#include <vector>
#include <functional>

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
    void Tick();
};
