#include "explosion.hpp"
#include "../application.hpp"
#include "../utils/math.hpp"

Explosion::Explosion(glm::vec3 position, float scale, float duration) {
    std::unique_ptr<Application>& app = Application::GetInstance();
    mesh = &app->graphicsBackend.globalMeshes.sphere;
    shader = &app->graphicsBackend.globalShaders.explosion;
    transform.position = position;
    transform.scale = glm::vec3(scale);
    spawnTimestamp = app->clock.currentTime;
    this->duration = duration;
    spawnScale = scale;
}

void Explosion::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    t = ((app->clock.currentTime - spawnTimestamp) / duration);
    transform.scale = MathUtils::Lerp<glm::vec3>(glm::vec3(spawnScale), glm::vec3(0.0), t);
}

void Explosion::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    app->graphicsBackend.BeginDrawMesh(*mesh, *shader, app->sceneManager.activeCamera, transform);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, glm::vec3(1.0f, 0.5f, 0.4f),"uStartColor");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, glm::vec3(0.3f),"uEndColor");
    app->graphicsBackend.UploadShaderUniformFloat(*shader, t, "uT");
    app->graphicsBackend.EndDrawMesh(*mesh);
}

void ExplosionSystemEntity::SpawnExplosion(glm::vec3 position, float scale, float duration) {
    std::unique_ptr<Application>& app = Application::GetInstance();
    Explosion* explosion = new Explosion(position, scale, duration);
    activeExplosions.push_back(explosion);

    Timer lifetime = Timer();
    lifetime.endTime = duration;
    lifetime.callback = [this, explosion]() {
        for(size_t i = 0; i < activeExplosions.size(); i++) {
            if(activeExplosions[i] == explosion) {
                activeExplosions.erase(activeExplosions.begin() + i);
                delete explosion;
                break;
            }
        }
    };
    app->clock.timers.push_back(lifetime);
}

void ExplosionSystemEntity::Update() {
    for(Explosion* e : activeExplosions) {
        e->Update();
    }
}

void ExplosionSystemEntity::Draw() {
    for(Explosion* e : activeExplosions) {
        e->Draw();
    }
}
