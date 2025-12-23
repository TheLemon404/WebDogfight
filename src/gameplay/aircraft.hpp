#pragma once

#include "../graphics/types.hpp"
#include "../graphics/backend.hpp"

#include <memory>
#include <future>
#include <iostream>

class Fuselage {
    protected:
    Mesh mesh;

    public:
    virtual void LoadAssets();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void UnloadAssets();
};

class FU13 : public Fuselage {
    protected:
    Mesh mesh;

    public:
    void LoadAssets() override;
    void Initialize() override {};
    void Update() override {};
    void Draw() override;
    void UnloadAssets() override;
};

class Wings {
    protected:
    Mesh mesh;

    public:
    virtual void LoadAssets();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void UnloadAssets();
};

class Engine {
    public:
    virtual void LoadAssets();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void UnloadAssets();
};

class Aircraft {
    public:
    std::unique_ptr<Fuselage> fuselage;
    std::unique_ptr<Wings> wings;
    std::unique_ptr<Engine> engine;

    Mesh testCube = Mesh(0,0,0,0);

    void LoadAssets() {
        /*
        fuselage->LoadAssets();
        wings->LoadAssets();
        engine->LoadAssets();
        */

        testCube = GraphicsBackend::CreateCube();
    }
    void Initialize() {
        /*
        fuselage->Initialize();
        wings->Initialize();
        engine->Initialize();
        */
    }
    void Update() {
        /*
        fuselage->Update();
        wings->Update();
        engine->Update();
        */
    }
    void Draw() {
        /*
        fuselage->Draw();
        wings->Draw();
        engine->Draw();
        */
    }
    void UnloadAssets() {
        /*
        fuselage->UnloadAssets();
        wings->UnloadAssets();
        engine->UnloadAssets();
        */
    }
};
