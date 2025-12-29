#include "gameplay/environment.hpp"
#include "gameplay/scene.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "graphics/loader.hpp"
#include "graphics/types.hpp"
#include "io/input.hpp"
#include "io/time.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


#ifdef __EMSCRIPTEN__
EM_JS(int, html_get_width, (), {
    return window.innerWidth;
})

EM_JS(int, html_get_height, (), {
    return window.innerHeight;
})
#endif

struct ApplicationSettings {
    int screenWidth = 800;
    int screenHeight = 600;
};

class Application {
public:
    unsigned int shaderProgram;
    unsigned int vao;

    ApplicationSettings settings;
    Window window;

    //this all needs to be moved into the scene later
    Shader* shader;
    SkeletalMesh* skeletalMesh;
    Camera* camera;
    Transform transform;
    Environment environment;

    std::unique_ptr<Scene> currentScene;

    bool Initialize() {
        currentScene->Initialize();

        return true;
    }

    void LoadAssets() {
        currentScene->LoadAssets();
    }

    void Update() {
        currentScene->Update();
    }

    void Draw() {
        currentScene->Draw();
    }

    void UnloadAssets() {
        currentScene->UnloadAssets();
    }
};

glm::vec3 rotatePointAroundPoint(
    const glm::vec3& pointToRotate,
    const glm::vec3& center,
    float angleRadians,
    const glm::vec3& axis)
{
    // 1. Translate the point to the origin (relative to the center)
    glm::vec3 translatedPoint = pointToRotate - center;

    // 2. Create the rotation quaternion
    glm::quat rotationQuat = glm::angleAxis(angleRadians, axis);

    // 3. Apply the rotation using quaternion multiplication
    glm::vec3 rotatedPoint = rotationQuat * translatedPoint;

    // 4. Translate the point back to its original position (relative to the center)
    glm::vec3 finalPoint = rotatedPoint + center;

    return finalPoint;
}

void main_loop(void* arg) {
    Application* app = static_cast<Application*>(arg);
    app->window.Poll();

    //this is probably not the best way to do this but who cares atm...
    app->camera->aspect = (float)app->window.width / app->window.height;
    if(InputManager::mouseButtonStates[GLFW_MOUSE_BUTTON_1] == GLFW_PRESS) {
        glm::vec3 cameraForward = glm::normalize(app->camera->target - app->camera->position);
        glm::vec3 cameraRight = glm::cross(glm::vec3(0.0, 1.0, 0.0), cameraForward);
        glm::vec3 horizontalAxis = rotatePointAroundPoint(app->camera->position, app->camera->target, InputManager::mouseDeltaY * Time::deltaTime, cameraRight);
        app->camera->position = rotatePointAroundPoint(horizontalAxis, app->camera->target, -InputManager::mouseDeltaX * Time::deltaTime, glm::vec3(0.0, 1.0, 0.0));
    }

    app->Update();
    app->Draw();

    app->transform.rotation.y += 0.1f;

    GraphicsBackend::ResetState(app->window.width, app->window.height);
    GraphicsBackend::SetDepthTest(true);
    GraphicsBackend::BeginDrawSkeletalMesh(*app->skeletalMesh, *app->shader, *app->camera, app->transform);
    GraphicsBackend::UploadShaderUniformVec3(*app->shader, app->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformVec3(*app->shader, app->environment.sunColor, "uSunColor");
    GraphicsBackend::EndDrawSkeletalMesh(*app->skeletalMesh);

    app->window.SwapBuffers();
    InputManager::ResetInputState();
    Time::Tick();
}

int main() {
    Application app = Application();
    app.window = Window();
#ifdef __EMSCRIPTEN__
    app.window.width = html_get_width();
    app.window.height = html_get_height();
#else
    app.window.width = app.settings.screenWidth;
    app.window.height = app.settings.screenHeight;
#endif
    app.window.title = "Fox2";

    app.currentScene = std::make_unique<TestScene>();

    if(app.Initialize()) {
        app.window.Open();
        app.LoadAssets();
    }

    Shader shader = GraphicsBackend::CreateShader("resources/shaders/skeletal.glsl");
    SkeletalMesh mesh = Loader::LoadSkeletalMeshFromGLTF("resources/meshes/demo_jet.gltf");
    Camera camera = Camera();
    camera.position = glm::vec3(10.0f, 10.0f, 10.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);

    app.shader = &shader;
    app.skeletalMesh = &mesh;
    app.camera = &camera;
    GraphicsBackend::SetBackfaceCulling(true);


#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, &app, 0, 1);
#else
    while(!app.window.ShouldClose()) {
        main_loop(&app);
    }
#endif

    glDeleteVertexArrays(1, &app.vao);
	glDeleteBuffers(1, &app.skeletalMesh->vbo);
	glDeleteProgram(app.shaderProgram);

    app.UnloadAssets();
    app.window.Close();

    return 0;
}
