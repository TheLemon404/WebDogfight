#include "widget.hpp"
#include "scene_manager.hpp"
#include <cmath>
#include "../io/time.hpp"
#include "../graphics/window.hpp"

void RectWidget::LoadResources() {
    quad = GraphicsBackend::CreateQuad();
    shader = GraphicsBackend::CreateShader("resources/shaders/ui_square.glsl");
}

void RectWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale);
    GraphicsBackend::UploadShaderUniformMat4(shader, WindowManager::GetUIOrthographicMatrix(), "uProjection");
    GraphicsBackend::UploadShaderUniformVec4(shader, color.value, "uColor");
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

void CircleWidget::Update() {
    thickness = ((1.0 + sin(Time::currentTime)) / 2.0) * 0.1f;
}

void CircleWidget::Draw() {
    GraphicsBackend::BeginDrawMesh2D(quad, shader, SceneManager::activeCamera, position, scale);
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
