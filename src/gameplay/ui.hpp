#pragma once

#include "../graphics/backend.hpp"

enum WidgetType {
    SQUARE,
    CIRCLE,
    RING,
};

class Widget {
    Mesh quad;
    Shader* shader;
    Color color;
    glm::vec2 screenPosition;
    glm::vec2 scale = glm::vec2(1.0f);
    const WidgetType type;

    public:
    const std::string name;
    const unsigned int id = rand();
    float radius = 0.5f;
    float thickness = 0.1f;

    Widget* parent;

    Widget(const std::string& name, const WidgetType type) : name(name), type(type) {};

    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};
