#include "scene.hpp"
#include "scene_manager.hpp"

void Scene::LoadResources() {
    environment.skybox = std::make_shared<Skybox>();

    for(std::shared_ptr<Entity>& entity : entities) {
        entity->LoadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->CreateWidgets();
        widgetLayer->LoadResources();
    }
}

void Scene::Initialize()  {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Initialize();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->Initialize();
    }
}

void Scene::Update()  {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Update();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UpdateLayer();
        widgetLayer->Update();
    }
}

void Scene::Draw() {
    if(environment.skybox) {
        GraphicsBackend::SetDepthMask(false);
        GraphicsBackend::SetBackfaceCulling(false);
        GraphicsBackend::DrawSkybox(*environment.skybox, SceneManager::activeCamera);
        GraphicsBackend::SetBackfaceCulling(true);
        GraphicsBackend::SetDepthMask(true);
    }
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Draw();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->Draw();
    }
}

void Scene::UnloadResources()  {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->UnloadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UnloadResources();
    }
}
