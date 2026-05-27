#include "missile.hpp"

#include "../application.hpp"

void HeatSeekingMissile::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = &app->graphicsBackend.globalMeshes.heatSeekingMissile;
    shader = &app->graphicsBackend.globalShaders.missile;
}

void RadarGuidedMissile::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = &app->graphicsBackend.globalMeshes.heatSeekingMissile;
    shader = &app->graphicsBackend.globalShaders.missile;
}
