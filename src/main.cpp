#include "gameplay/scene.hpp"
#include "graphics/window.hpp"

struct ApplicationSettings {
    int screenWidth = 800;
    int screenHeight = 600;
};

class Application {
public:
    ApplicationSettings settings;
    Scene currentScene;

    bool Initialize() {
        currentScene.Initialize();

        return true;
    }

    void LoadAssets() {
        currentScene.LoadAssets();
    }

    void Update() {
        currentScene.Update();
    }

    void Draw() {
        currentScene.Draw();
    }

    void UnloadAssets() {
        currentScene.UnloadAssets();
    }
};

int main() {
    Application app = Application();
    Window window = Window(app.settings.screenWidth, app.settings.screenHeight, "Fox2");

    if(app.Initialize()) {
        window.Open();
    }

    while(!window.ShouldClose()) {
        app.Update();
        app.Draw();

        window.PollAndSwap();
    }

    app.UnloadAssets();
    window.Close();

    return 0;
}
