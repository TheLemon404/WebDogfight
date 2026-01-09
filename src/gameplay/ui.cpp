#include "ui.hpp"
#include "scene_manager.hpp"
#include <cmath>
#include "../io/time.hpp"
#include "../graphics/window.hpp"

void Widget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    switch(type) {
        case WidgetType::SQUARE:
            shader = &GraphicsBackend::globalShaders.uiSquare;
            break;
        case WidgetType::CIRCLE:
            shader = &GraphicsBackend::globalShaders.uiCircle;
            break;
        case WidgetType::RING:
            shader = &GraphicsBackend::globalShaders.uiRing;
            break;
        default:
            break;
    }
}

void Widget::Initialize() {

}

void Widget::Update() {
    thickness = ((1.0 + sin(Time::currentTime)) / 2.0) * 0.1f;
}

void Widget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, *shader, SceneManager::activeCamera, screenPosition, scale);
    GraphicsBackend::UploadShaderUniformMat4(*shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    if(type == WidgetType::CIRCLE || type == WidgetType::RING) {
        GraphicsBackend::UploadShaderUniformFloat(*shader, radius, "uRadius");
    }
    if(type == WidgetType::RING) {
        GraphicsBackend::UploadShaderUniformFloat(*shader, thickness, "uThickness");
    }
    GraphicsBackend::UploadShaderUniformVec4(*shader, color.value, "uColor");
    GraphicsBackend::EndDrawMesh2D(quad);
}

void Widget::UnloadResources() {

}
