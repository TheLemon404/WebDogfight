#include "scene.hpp"
#include "aircraft.hpp"

void Scene::Initialize() {
    for(Aircraft& aircraft : aircrafts) {
        aircraft.Initialize();
    }
}

void Scene::Update() {
    for(Aircraft& aircraft : aircrafts) {
        aircraft.Update();
    }
}

void Scene::Draw() {
    for(Aircraft& aircraft : aircrafts) {
        aircraft.Draw();
    }
}

void Scene::UnloadAssets() {
    for(Aircraft& aircraft : aircrafts) {
        aircraft.UnloadAssets();
    }
}
