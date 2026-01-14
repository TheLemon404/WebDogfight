#include "scene.hpp"
#include "scene_manager.hpp"

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
