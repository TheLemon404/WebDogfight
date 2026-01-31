#include "widget.hpp"
#include "GLFW/glfw3.h"
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

bool RectWidget::IsHovered() {
    glm::vec2 fractionMousePosition = InputManager::mousePosition / glm::vec2(WindowManager::primaryWindow->width, WindowManager::primaryWindow->height);
    fractionMousePosition -= glm::vec2(0.5f);
    fractionMousePosition *= 2.0f;
    std::cout << glm::to_string(InputManager::mousePosition) << " -> " << WindowManager::primaryWindow->width << " : " << WindowManager::primaryWindow->height << std::endl;
    return -scale.x - position.x <= fractionMousePosition.x && -scale.y - position.y <= fractionMousePosition.y && scale.x - position.x >= fractionMousePosition.x && scale.y - position.y >= fractionMousePosition.y;
}

void RectWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = &GraphicsBackend::globalShaders.uiSquare;
}

void RectWidget::Draw() {
    if(InputManager::IsKeyPressed(GLFW_KEY_T)) return;

    GraphicsBackend::BeginDrawMesh2D(quad, *shader, position, scale, rotation, stretchWithAspectRatio);
    GraphicsBackend::UploadShaderUniformVec4(*shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(*shader, border, "uBorder");
    GraphicsBackend::UploadShaderUniformInt(*shader, cornerBorder, "uCornerBorder");
    GraphicsBackend::UploadShaderUniformInt(*shader, cornerLength, "uCornerLength");
    GraphicsBackend::UploadShaderUniformVec4(*shader, borderColor.value, "uBorderColor");
    GraphicsBackend::UploadShaderUniformVec4(*shader, cornerColor.value, "uCornerColor");
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / (stretchWithAspectRatio ? 1.0f : WindowManager::aspect), WindowManager::primaryWindow->height * scale.y);
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
    //---IMPORTANT--- REMOVE THIS
    if(InputManager::IsKeyPressed(GLFW_KEY_T)) return;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float x = 0.0f;
    float y = 0.0f;

    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x * (stretchWithAspectRatio ? WindowManager::aspect : 1.0f), WindowManager::primaryWindow->height * scale.y);

    unsigned int indexOffset = 0;

    for(std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if(*c == '\n'){
            y -= font.lineHeight;
            x = 0.0f;
            continue;
        }

        float yScale = scale.y / font.fontScale * 1000.0f;
        float xScale = scale.x / font.fontScale * 1000.0f;

        Character ch = font.characters[*c];
        float xPos;
        float yPos;
        if(centerText) {
            //---TODO--- impliment font centering
        }
        else {
            xPos = (x + ch.bearing.x) - xScale + font.tabIn;
            yPos = (y + ch.bearing.y - ch.size.y) + yScale - font.lineHeight;
        }
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
    if(InputManager::IsKeyPressed(GLFW_KEY_T)) return;

    GraphicsBackend::SetDepthTest(false);
    RectWidget::Draw();

    GraphicsBackend::BeginDrawMesh2D(textMesh, *textShader, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformInt(*textShader, 0, "uFontTexture");
    GraphicsBackend::UploadShaderUniformVec4(*textShader, fontColor.value, "uColor");
    GraphicsBackend::UseTextureIDSlot(font.atlasTextureID, 0);
    GraphicsBackend::EndDrawMesh2D(textMesh);

    GraphicsBackend::SetDepthTest(true);
}

void TextRectWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(textMesh);
}

void TextButtonWidget::Draw() {
    TextRectWidget::Draw();

    if(IsHovered()) {
        if(InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)){
            color.value = glm::vec4(0.2);
            onPressed();
        }
        else {
            color.value = glm::vec4(0.3);
        }
    }
    else {
        color.value = glm::vec4(0.4);
    }
}

void InputWidget::Draw() {
    if(InputManager::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_1)){
        if(IsHovered()) {
            SetText("");
            focused = true;
        }
        else {
            focused = false;
        }
    }

    if(focused) {
        color.value = glm::vec4(0.2);
    }
    else {
        color.value = glm::vec4(0.4);
    }

    TextRectWidget::Draw();
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
    glm::ivec2 widgetResolution = glm::ivec2(WindowManager::primaryWindow->width * scale.x / WindowManager::aspect, WindowManager::primaryWindow->height * scale.y);
    GraphicsBackend::UploadShaderUniformIVec2(*shader, widgetResolution, "uWidgetResolution");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void CircleWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
}
