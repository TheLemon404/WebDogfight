#pragma once

#include <cstdint>

class Entity {
    public:
    virtual void LoadResources() = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnloadResources() = 0;

    virtual ~Entity() {}
};
