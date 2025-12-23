#include "aircraft.hpp"

void FU13::LoadAssets() {

}

void FU13::Draw() {

}

void FU13::UnloadAssets() {

}

void Aircraft::LoadAssets() {
    fuselage->LoadAssets();
    wings->LoadAssets();
    engine->LoadAssets();
}

void Aircraft::Initialize() {
    fuselage->Initialize();
    wings->Initialize();
    engine->Initialize();
}

void Aircraft::Update() {
    fuselage->Update();
    wings->Update();
    engine->Update();
}

void Aircraft::Draw() {
    fuselage->Draw();
    wings->Draw();
    engine->Draw();
}

void Aircraft::UnloadAssets() {
    fuselage->UnloadAssets();
    wings->UnloadAssets();
    engine->UnloadAssets();
}
