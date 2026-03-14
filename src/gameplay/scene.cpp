#include "scene.hpp"
#include "entity.hpp"
#include "scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "widget.hpp"
#include <memory>

void Scene::LoadResourcesAsync() {
    isLoadingResources = true;
    switch(resourceLoadingState) {
        case BEGIN:
            {
                environment.skybox = std::make_shared<Skybox>();
                resourceLoadingState = LOADING_ENTITIES;
            }
            break;
        case LOADING_ENTITIES:
            {
                entities[resourceLoadingIndex]->LoadResources();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == entities.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = CREATING_UI;
                }
            }
            break;
        case CREATING_UI:
            {
                widgetLayers[resourceLoadingIndex]->CreateWidgets();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == widgetLayers.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = LOADING_UI;
                }
            }
            break;
        case LOADING_UI:
            {
                widgetLayers[resourceLoadingIndex]->LoadResources();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == widgetLayers.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = END;
                }
            }
            break;
        case END:
            {
                isLoadingResources = false;
                if(onResourcesLoadedCallback) onResourcesLoadedCallback();
            }
            break;
    }
}

void Scene::LoadResources() {
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
        FOX2_PROFILE_FUNCTION()
        GraphicsBackend::SetDepthMask(false);
        GraphicsBackend::SetBackfaceCulling(false);
        GraphicsBackend::DrawSkybox(*environment.skybox, SceneManager::activeCamera);
        GraphicsBackend::SetBackfaceCulling(true);
        GraphicsBackend::SetDepthMask(true);
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
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->UnloadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UnloadResources();
    }
}
