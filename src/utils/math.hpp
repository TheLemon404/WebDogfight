#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/scalar_multiplication.hpp>

#define GLOBAL_FORWARD glm::vec3(0.0, 0.0, 1.0)
#define GLOBAL_LEFT glm::vec3(1.0, 0.0, 0.0)
#define GLOBAL_UP glm::vec3(0.0, 1.0, 0.0)
#define GLOBAL_ZERO glm::vec3(0.0)
#define PI 3.141592

class MathUtils {
    public:
    template<typename T>
    static T Lerp(T from, T to, double factor) {
        return (1.0 - factor) * from + factor * to;
    };

    template<typename T>
    static T Clamp(T val, T min, T max) {
        if(val < min) return min;
        else if(val > max) return max;
        return val;
    }

    template<typename T>
    static T Min(T val, T min) {
        if(val < min) return min;
        return val;
    }

    template<typename T>
    static T Max(T val, T max) {
        if(val > max) return max;
        return val;
    }


    static glm::vec3 RotatePointAroundPoint(const glm::vec3& pointToRotate, const glm::vec3& center, float angleRadians, const glm::vec3& axis) {
        // 1. Translate the point to the origin (relative to the center)
        glm::vec3 translatedPoint = pointToRotate - center;

        // 2. Create the rotation quaternion
        glm::quat rotationQuat = glm::angleAxis(angleRadians, axis);

        // 3. Apply the rotation using quaternion multiplication
        glm::vec3 rotatedPoint = rotationQuat * translatedPoint;

        // 4. Translate the point back to its original position (relative to the center)
        glm::vec3 finalPoint = rotatedPoint + center;

        return finalPoint;
    }
};
