#include "types.hpp"
#include "backend.hpp"

Skybox::Skybox() {
    mesh = GraphicsBackend::CreateCube();
    shader = GraphicsBackend::CreateShader("resources/shaders/skybox.glsl");
    skyColor.value = glm::vec4(0.68f, 0.85f, 0.90f, 1.0);
    horizonColor.value = glm::vec4(1.0f, 0.75f, 0.55f, 1.0f);
}

Skybox::~Skybox() {
    GraphicsBackend::DeleteMesh(mesh);
    GraphicsBackend::DeleteShader(shader);
}
