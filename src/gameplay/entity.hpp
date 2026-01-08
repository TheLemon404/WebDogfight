#pragma once

#include <cstdlib>
#include <string>

class Entity {
    public:
    const unsigned int id = rand();
    const std::string name;

    virtual void LoadResources() = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnloadResources() = 0;

    Entity(const std::string& name) : name(name) {}
    virtual ~Entity() {}
};
