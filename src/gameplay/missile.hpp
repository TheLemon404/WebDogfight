#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

class Missile : public Entity{
    protected:
    Shader* shader;
    Mesh* mesh;

    public:
    Missile(const std::string& name) : Entity(name) {};
};

class HeatSeekingMissile : public Missile {
    public:
    void LoadResources() override;

    HeatSeekingMissile(const std::string& name) : Missile(name) {};
};

class RadarGuidedMissile : public Missile {
    public:
    void LoadResources() override;

    RadarGuidedMissile(const std::string& name) : Missile(name) {};
};
