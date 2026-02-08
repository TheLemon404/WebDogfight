#include "scene.hpp"
#include "scene_manager.hpp"
#include "../utils/instrumentor.hpp"

void Scene::LoadResources() {
    environment.skybox = std::make_shared<Skybox>();

    for(CloudsVolume& cloudVolume : environment.cloudVolumes) {
        cloudVolume.LoadResources();
    }

    for(std::shared_ptr<Entity>& entity : entities) {
        entity->LoadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->CreateWidgets();
        widgetLayer->LoadResources();
    }
}

void Scene::Initialize()  {
    for(CloudsVolume& cloudVolume : environment.cloudVolumes) {
        cloudVolume.Initialize();
    }

    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Initialize();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->Initialize();
    }
}

void Scene::Update()  {
    for(CloudsVolume& cloudVolume : environment.cloudVolumes) {
        cloudVolume.Update();
    }

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
        FOX2_PROFILE_FUNCTION()
        GraphicsBackend::SetDepthMask(false);
        GraphicsBackend::SetBackfaceCulling(false);
        GraphicsBackend::DrawSkybox(*environment.skybox, SceneManager::activeCamera);
        GraphicsBackend::SetBackfaceCulling(true);
        GraphicsBackend::SetDepthMask(true);
    }

    for(CloudsVolume& cloudVolume : environment.cloudVolumes) {
        cloudVolume.Draw();
    }
    for(std::shared_ptr<Entity>& entity : entities) {
        FOX2_PROFILE_SCOPE(entity->name.c_str())
        entity->Draw();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        FOX2_PROFILE_SCOPE("widget layer")
        widgetLayer->Draw();
    }
}

void Scene::UnloadResources()  {
    for(CloudsVolume& cloudVolume : environment.cloudVolumes) {
        cloudVolume.UnloadResources();
    }

    for(std::shared_ptr<Entity>& entity : entities) {
        entity->UnloadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UnloadResources();
    }
}
