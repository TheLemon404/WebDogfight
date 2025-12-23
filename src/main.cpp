#include <emscripten.h>

#include "gameplay/scene.hpp"
#include <GLES3/gl3.h>
#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <gl/gl.h>
#include <memory>

struct ApplicationSettings {
    int screenWidth = 800;
    int screenHeight = 600;
};

class Application {
public:
    ApplicationSettings settings;
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

Application app = Application();
Window window = Window(app.settings.screenWidth, app.settings.screenHeight, "Fox2");

void main_loop() {
    app.Update();
    app.Draw();

    float val = 225.0 / 255.0;
    glClearColor(val, val, val, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    window.PollAndSwapBuffers();
}

int main() {
    app.currentScene = std::make_unique<TestScene>();

    if(app.Initialize()) {
        window.Open();
        app.LoadAssets();
    }

    emscripten_set_main_loop(main_loop, 0, 1);

    app.UnloadAssets();
    window.Close();

    return 0;
}
