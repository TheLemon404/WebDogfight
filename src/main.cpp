#include "gameplay/scene.hpp"
#include "graphics/types.hpp"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include <glad/glad.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>

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
    Shader* shader;
    Mesh* mesh;
    Camera* camera;
    Transform transform;
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

void main_loop(void* arg) {
    Application* app = static_cast<Application*>(arg);

    app->Update();
    app->Draw();

    float val = 25.0 / 255.0;

   	glViewport(0, 0, app->window.width, app->window.height);

    glClearColor(val, val, val, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    app->transform.rotation.y += 0.1f;

    GraphicsBackend::DrawMesh(*app->mesh, *app->shader, *app->camera, app->transform);

    app->window.PollAndSwapBuffers();
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

    Shader shader = GraphicsBackend::CreateShader("resources/shaders/test.glsl");
    Mesh mesh = GraphicsBackend::CreateCube();
    Camera camera = Camera();
    camera.position = glm::vec3(3.0f, 3.0f, 3.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
#ifdef __EMSCRIPTEN__
    camera.aspect = (float)html_get_width() / html_get_height();
#else
    camera.aspect = (float)app.window.width / app.window.height;
#endif

    app.shader = &shader;
    app.mesh = &mesh;
    app.camera = &camera;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, &app, 0, 1);
#else
    while(!app.window.ShouldClose()) {
        main_loop(&app);
    }
#endif

    glDeleteVertexArrays(1, &app.vao);
	glDeleteBuffers(1, &app.mesh->vbo);
	glDeleteProgram(app.shaderProgram);

    app.UnloadAssets();
    app.window.Close();

    return 0;
}
