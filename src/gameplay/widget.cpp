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
    shader = GraphicsBackend::CreateShader("resources/shaders/ui_square.glsl");
}

void RectWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformMat4(shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    GraphicsBackend::UploadShaderUniformVec4(shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformInt(shader, border, "uBorder");
    GraphicsBackend::UploadShaderUniformInt(shader, cornerBorder, "uCornerBorder");
    GraphicsBackend::UploadShaderUniformInt(shader, cornerLength, "uCornerLength");
    GraphicsBackend::UploadShaderUniformVec4(shader, borderColor.value, "uBorderColor");
    GraphicsBackend::UploadShaderUniformVec4(shader, cornerColor.value, "uCornerColor");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void RectWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
    GraphicsBackend::DeleteShader(shader);
}

void CircleWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = GraphicsBackend::CreateShader("resources/shaders/ui_circle.glsl");
}

void CircleWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale, rotation);
    GraphicsBackend::UploadShaderUniformMat4(shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    GraphicsBackend::UploadShaderUniformVec4(shader, color.value, "uColor");
    GraphicsBackend::UploadShaderUniformFloat(shader, radius, "uRadius");
    GraphicsBackend::UploadShaderUniformFloat(shader, thickness, "uThickness");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void CircleWidget::UnloadResources() {
    GraphicsBackend::DeleteMesh(quad);
    GraphicsBackend::DeleteShader(shader);
}
