#pragma once

#include "../graphics/loader.hpp"
#include <memory>

class Widget {
    protected:
    Mesh quad;
    Shader* shader;

    public:
    Color color;
    float rotation = 0.0;
    glm::vec2 position = glm::vec2(0.0f);
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

    virtual void CreateWidgets() = 0;
    virtual void UpdateLayer() = 0;

    std::shared_ptr<Widget> GetWidgetByName(const std::string& name);
    std::shared_ptr<Widget> GetWidgetById(const unsigned int id);

    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};

class RectWidget : public Widget {
    protected:
    bool IsHovered();

    public:
    bool stretchWithAspectRatio = false;

    int border = 2;
    int cornerBorder = 2;
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
    protected:
    Shader* textShader;
    Mesh textMesh;
    std::string text;

    void RecomputeTextMesh();

    public:
    Font font;

    //--- IMPORTANT --- this only works with 1 line text length
    bool centerText = false;

    Color fontColor = COLOR_WHITE;

    void SetText(const std::string& text) {
        this->text = text;
        RecomputeTextMesh();
    }

    void LoadResources() override;
    void Draw() override;
    void UnloadResources() override;

    TextRectWidget(const std::string& name, Font font) : font(font), RectWidget(name) {}
};

class TextButtonWidget : public TextRectWidget {
    public:
    void Draw() override;

    std::function<void()> onPressed;

    TextButtonWidget(const std::string& name, Font font) : TextRectWidget(name, font) {}
};

class InputWidget : public TextRectWidget {
    public:
    bool focused = false;

    void Draw() override;

    InputWidget(const std::string& name, Font font) : TextRectWidget(name, font) {}
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
