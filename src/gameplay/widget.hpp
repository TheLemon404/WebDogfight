#pragma once

#include "../graphics/loader.hpp"
#include <memory>

class Widget {
    protected:
    Mesh quad;
    Shader shader;

    public:
    Color color;
    float rotation = 0.0;
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

class WidgetLayer {
    public:
    std::vector<std::shared_ptr<Widget>> widgets;

    virtual void CreateWidgets() {};
    virtual void UpdateLayer() {};

    std::shared_ptr<Widget> GetWidgetByName(const std::string& name);
    std::shared_ptr<Widget> GetWidgetById(const unsigned int id);

    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};

class RectWidget : public Widget {
    public:
    int border = 1;
    int cornerBorder = 1;
    int cornerLength = 10;
    Color borderColor;
    Color cornerColor;

    void LoadResources() override;
    void Initialize() override {};
    void Update() override {};
    void Draw() override;
    void UnloadResources() override;

    RectWidget(const std::string& name) : Widget(name) {
        borderColor.value = glm::vec4(1.0);
        cornerColor.value = glm::vec4(1.0);
    };
};

class TextRectWidget : public RectWidget {
    Shader textShader;
    Mesh textMesh;

    bool draw = true;

    std::vector<Transform> transforms;

    public:
    Font font;

    Color fontColor = COLOR_WHITE;

    std::string text;

    void LoadResources() override;
    void Draw() override;
    void UnloadResources() override;

    TextRectWidget(const std::string& name, Font font) : font(font), RectWidget(name) {}
};

class CircleWidget : public Widget {
    public:
    int radius = 15;
    int thickness = 2;

    void LoadResources() override;
    void Initialize() override {};
    void Update() override {};
    void Draw() override;
    void UnloadResources() override;

    CircleWidget(const std::string& name) : Widget(name) {};
};
