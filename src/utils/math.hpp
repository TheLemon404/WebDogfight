#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/scalar_multiplication.hpp>

class MathUtils {
    public:
    template<typename T>
    static T Lerp(T from, T to, float factor) {
        return (1.0 - factor) * from + factor * to;
    };

    template<typename T>
    static T Clamp(T val, T min, T max) {
        if(val < min) return min;
        else if(val > max) return max;
        return val;
    }
};
