#pragma once

#include <memory>
#include <future>

class Fuselage {
public:
    virtual void LoadAssets();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void UnloadAssets();
};

class FU13 : public Fuselage {
public:
    void LoadAssets() override;
    void Initialize() override {};
    void Update() override {};
    void Draw() override;
    void UnloadAssets() override;
};

class Wings {
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
    std::shared_ptr<Fuselage> fuselage;
    std::shared_ptr<Wings> wings;
    std::shared_ptr<Engine> engine;

    void LoadAssets();
    void Initialize();
    void Update();
    void Draw();
    void UnloadAssets();
};
