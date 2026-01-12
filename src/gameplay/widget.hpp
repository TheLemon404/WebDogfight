#pragma once

#include "../graphics/backend.hpp"
#include <memory>

class Widget {
    protected:
    Mesh quad;
    Shader shader;

    public:
    Color color;
    glm::vec2 position;
    glm::vec2 scale = glm::vec2(1.0f);

    const std::string name;
    const unsigned int id = rand();

    Widget* parent;

    Widget(const std::string& name) : name(name) {};

    virtual void LoadResources() = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnloadResources() = 0;
};

class RectWidget : public Widget {
    public:
    void LoadResources() override;
    void Initialize() override {};
    void Update() override {};
    void Draw() override;
    void UnloadResources() override;

    RectWidget(const std::string& name) : Widget(name) {};
};

class CircleWidget : public Widget {
    protected:
    float radius = 0.5f;
    float thickness = 0.1f;

    public:
    void LoadResources() override;
    void Initialize() override {};
    void Update() override;
    void Draw() override;
    void UnloadResources() override;

    CircleWidget(const std::string& name) : Widget(name) {};
};
