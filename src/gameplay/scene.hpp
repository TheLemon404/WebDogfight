#pragma once

#include "aircraft.hpp"
#include <string>
#include <vector>

class Scene {
    protected:
    std::vector<Aircraft> aircrafts;

    public:
    virtual void LoadAssets() {
        for(Aircraft& aircraft : aircrafts) {
            aircraft.LoadAssets();
        }
    }
    virtual void Initialize() {
        for(Aircraft& aircraft : aircrafts) {
            aircraft.Initialize();
        }
    }

    virtual void Update() {
        for(Aircraft& aircraft : aircrafts) {
            aircraft.Update();
        }
    }
    virtual void Draw() {
        for(Aircraft& aircraft : aircrafts) {
            aircraft.Draw();
        }
    }
    virtual void UnloadAssets() {
        for(Aircraft& aircraft : aircrafts) {
            aircraft.UnloadAssets();
        }
    }
};

class TestScene : public Scene {
    void Initialize() override {
        aircrafts.push_back({
        });

        Scene::Initialize();
    }

    void Update() override {
        Scene::Update();
    }

    void Draw() override {
        Scene::Draw();
    }

    void UnloadAssets() override {
        Scene::UnloadAssets();
    }
};
