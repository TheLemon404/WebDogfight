#include "widget.hpp"
#include "scene_manager.hpp"
#include <cmath>
#include "../io/time.hpp"
#include "../graphics/window.hpp"

std::shared_ptr<Widget> WidgetLayer::GetWidgetByName(const std::string& name) {
    for(std::shared_ptr<Widget>& widget : widgets) {
        if(widget->name == name) return widget;
    }

    return nullptr;
}

std::shared_ptr<Widget> WidgetLayer::GetWidgetById(const unsigned int id) {
    for(std::shared_ptr<Widget>& widget : widgets) {
        if(widget->id == id) return widget;
    }

    return nullptr;
}

void WidgetLayer::LoadResources() {
    for(std::shared_ptr<Widget>& widget : widgets) {
        widget->LoadResources();
    }
}

void WidgetLayer::Initialize() {
    for(std::shared_ptr<Widget>& widget : widgets) {
        widget->Initialize();
    }
}

void WidgetLayer::Update() {
    for(std::shared_ptr<Widget>& widget : widgets) {
        widget->Update();
    }
}

void WidgetLayer::Draw() {
    for(std::shared_ptr<Widget>& widget : widgets) {
        widget->Draw();
    }
}

void WidgetLayer::UnloadResources() {
    for(std::shared_ptr<Widget>& widget : widgets) {
        widget->UnloadResources();
    }
}

void RectWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = Loader::LoadShaderFromGLSL("resources/shaders/ui_square.glsl");
}

void RectWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformVec4(shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(shader, border, "uBorder");
    GraphicsBackend::UploadShaderUniformInt(shader, cornerBorder, "uCornerBorder");
    GraphicsBackend::UploadShaderUniformInt(shader, cornerLength, "uCornerLength");
    GraphicsBackend::UploadShaderUniformVec4(shader, borderColor.value, "uBorderColor");
    GraphicsBackend::UploadShaderUniformVec4(shader, cornerColor.value, "uCornerColor");
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);
    GraphicsBackend::UploadShaderUniformIVec2(shader, widgetResolution, "uWidgetResolution");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void RectWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
    GraphicsBackend::DeleteShader(shader);
}

void TextRectWidget::LoadResources() {
    RectWidget::LoadResources();

    textShader = Loader::LoadShaderFromGLSL("resources/shaders/font.glsl");
    textMesh = GraphicsBackend::CreateQuad();
}

void TextRectWidget::Draw() {
    GraphicsBackend::SetDepthTest(false);
    RectWidget::Draw();

    float x = 0.0f;
    float y = 0.0f;

    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);

    for(std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if(*c == '\n'){
            y -= font.lineHeight;
            x = 0.0f;
            continue;
        }

        Character ch = font.characters[*c];
        float xPos = (x + ch.bearing.x) - widgetResolution.x + font.tabIn;
        float yPos = (y + ch.size.y - ch.bearing.y) + widgetResolution.y - font.lineHeight;
        float w = ch.size.x;
        float h = ch.size.y;

        float yScale = WindowManager::primaryWindow->height;
        float xScale = (WindowManager::primaryWindow->width / WindowManager::widthFraction) * 1.5f;

        Vertex vertices[4] = {
            {{ xPos / xScale,  yPos / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 0
            {{ (xPos + w) / xScale, yPos / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 1
            {{ (xPos + w) / xScale, (yPos + h) / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 2
            {{ xPos / xScale,  (yPos + h) / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 3
        };

        GraphicsBackend::UpdateMeshVerticesPositions(textMesh, vertices, 4);

        glm::vec2 finalScale = scale * font.fontScale;
        GraphicsBackend::BeginDrawMesh2D(textMesh, textShader, SceneManager::activeCamera, position, finalScale, rotation);
        GraphicsBackend::UploadShaderUniformInt(textShader, 0, "uFontTexture");
        GraphicsBackend::UploadShaderUniformVec4(textShader, fontColor.value, "uColor");
        GraphicsBackend::UseTextureIDSlot(ch.textureID, 0);
        GraphicsBackend::EndDrawMesh2D(textMesh);

        x += ch.advance >> 6;
    }

    GraphicsBackend::SetDepthTest(true);
}

void TextRectWidget::UnloadResources() {
    GraphicsBackend::DeleteShader(textShader);
    GraphicsBackend::DeleteMesh(textMesh);
    GraphicsBackend::DeleteFont(font);
}

void CircleWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = Loader::LoadShaderFromGLSL("resources/shaders/ui_circle.glsl");
}

void CircleWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformMat4(shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    GraphicsBackend::UploadShaderUniformVec4(shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(shader, radius, "uRadius");
    GraphicsBackend::UploadShaderUniformInt(shader, thickness, "uThickness");
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);
    GraphicsBackend::UploadShaderUniformIVec2(shader, widgetResolution, "uWidgetResolution");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void CircleWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
    GraphicsBackend::DeleteShader(shader);
}
