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
    shader = &GraphicsBackend::globalShaders.uiSquare;
}

void RectWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, *shader, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformVec4(*shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(*shader, border, "uBorder");
    GraphicsBackend::UploadShaderUniformInt(*shader, cornerBorder, "uCornerBorder");
    GraphicsBackend::UploadShaderUniformInt(*shader, cornerLength, "uCornerLength");
    GraphicsBackend::UploadShaderUniformVec4(*shader, borderColor.value, "uBorderColor");
    GraphicsBackend::UploadShaderUniformVec4(*shader, cornerColor.value, "uCornerColor");
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);
    GraphicsBackend::UploadShaderUniformIVec2(*shader, widgetResolution, "uWidgetResolution");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void RectWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
}

void TextRectWidget::LoadResources() {
    RectWidget::LoadResources();

    textShader = &GraphicsBackend::globalShaders.font;
    textMesh = GraphicsBackend::CreateQuad();

    RecomputeTextMesh();
}

void TextRectWidget::RecomputeTextMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float x = 0.0f;
    float y = 0.0f;

    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);

    unsigned int indexOffset = 0;

    for(std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if(*c == '\n'){
            y -= font.lineHeight;
            x = 0.0f;
            continue;
        }

        float yScale = scale.y * 1000.0f;
        float xScale = scale.x * 1000.0f;

        Character ch = font.characters[*c];
        float xPos = (x + ch.bearing.x) - xScale + font.tabIn;
        float yPos = (y + ch.bearing.y - ch.size.y) + yScale - font.lineHeight;
        float w = ch.size.x;
        float h = ch.size.y;

        Vertex v1 = {{ xPos / xScale,  yPos / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {ch.minUV.x, ch.maxUV.y}};
        Vertex v2 = {{ (xPos + w) / xScale, yPos / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {ch.maxUV.x, ch.maxUV.y}};
        Vertex v3 = {{ (xPos + w) / xScale, (yPos + h) / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {ch.maxUV.x, ch.minUV.y}};
        Vertex v4 = {{ xPos / xScale,  (yPos + h) / yScale,  0.0}, {0.0f, 0.0f, 1.0f}, {ch.minUV.x, ch.minUV.y}};

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);

        indices.push_back(indexOffset + 0);
        indices.push_back(indexOffset + 1);
        indices.push_back(indexOffset + 2);
        indices.push_back(indexOffset + 2);
        indices.push_back(indexOffset + 3);
        indices.push_back(indexOffset + 0);

        x += ch.advance >> 6;

        indexOffset += 4;
    }

    GraphicsBackend::UpdateMeshVertices(textMesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void TextRectWidget::Draw() {
    // --- IMPORTANT --- THIS IS A BANDAID, REMOVE THIS ASAP!!!
    if(InputManager::IsKeyJustPressed(GLFW_KEY_T)) draw = !draw;
    if(!draw) {
        return;
    }

    GraphicsBackend::SetDepthTest(false);
    RectWidget::Draw();

    glm::vec2 finalScale = scale * font.fontScale;
    GraphicsBackend::BeginDrawMesh2D(textMesh, *textShader, position, finalScale, rotation);
    GraphicsBackend::UploadShaderUniformInt(*textShader, 0, "uFontTexture");
    GraphicsBackend::UploadShaderUniformVec4(*textShader, fontColor.value, "uColor");
    GraphicsBackend::UseTextureIDSlot(font.atlasTextureID, 0);
    GraphicsBackend::EndDrawMesh2D(textMesh);

    GraphicsBackend::SetDepthTest(true);
}

void TextRectWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(textMesh);
    GraphicsBackend::DeleteFont(font);
}

void CircleWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = &GraphicsBackend::globalShaders.uiCircle;
}

void CircleWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, *shader, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformMat4(*shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    GraphicsBackend::UploadShaderUniformVec4(*shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(*shader, radius, "uRadius");
    GraphicsBackend::UploadShaderUniformInt(*shader, thickness, "uThickness");
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::widthFraction, WindowManager::primaryWindow->height * scale.y);
    GraphicsBackend::UploadShaderUniformIVec2(*shader, widgetResolution, "uWidgetResolution");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void CircleWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
}
