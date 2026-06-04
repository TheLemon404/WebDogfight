#include "test_scene.hpp"
#include "GLFW/glfw3.h"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"
#include "../networking/network_manager.hpp"
#include "../application.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "../utils/math.hpp"

void MenuWidgetLayer::CreateWidgets() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<RectWidget> background = CreateWidget<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 0.0);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);

    nameInput = CreateWidget<InputWidget>("nameInput", app->graphicsBackend.globalFonts.defaultFont);
    nameInput->stretchWithAspectRatio = true;
    nameInput->moveWithAspectRatio = true;
    nameInput->centerText = true;
    nameInput->maxCharacters = 6;
    nameInput->SetText("guest");
    nameInput->font.fontScale = 2.0;
    nameInput->scale = glm::vec2(0.2, 0.09);
    nameInput->position.y = 0.1f;
    nameInput->color.value = glm::vec4(0.2);
    nameInput->borderColor.value = glm::vec4(0.4);
    nameInput->cornerColor.value = glm::vec4(0.7);
    nameInput->onUnFocus = [this, &app] {
        if(app->networkManager.IsConnected() && app->networkManager.GetLobbyId() != -1) {
            app->networkManager.networkGameState.clientStates[app->networkManager.localClientId].name = nameInput->GetText();
        }
    };

    std::shared_ptr<TextButtonWidget> playButton = CreateWidget<TextButtonWidget>("playButton", app->graphicsBackend.globalFonts.defaultFont);
    playButton->stretchWithAspectRatio = true;
    playButton->moveWithAspectRatio = true;
    playButton->centerText = true;
    playButton->SetText("Play");
    playButton->font.fontScale = 2.0;
    playButton->scale = glm::vec2(0.2, 0.09);
    playButton->position.y = -0.1f;
    playButton->color.value = glm::vec4(0.2);
    playButton->borderColor.value = glm::vec4(0.4);
    playButton->cornerColor.value = glm::vec4(0.7);
    playButton->onPressed = [this, &app]{
        #ifdef __EMSCRIPTEN__
        if(app->networkManager.IsConnected()) {
            InputManager::mouseHidden = true;
            glfwSetInputMode(app->windowManager.primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            std::shared_ptr<Aircraft> aircraft = std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json", app->networkManager.localClientId);
            app->sceneManager.currentScene->RuntimeSpawn(aircraft);
            this->invisible = true;
        }
        #else
        InputManager::mouseHidden = true;
        glfwSetInputMode(app->windowManager.primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        std::shared_ptr<Aircraft> aircraft = std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json", app->networkManager.localClientId);
        app->sceneManager.currentScene->RuntimeSpawn(aircraft);
        this->invisible = true;
        #endif
    };

    std::shared_ptr<TextButtonWidget> lobbyButton = CreateWidget<TextButtonWidget>("lobbyButton", app->graphicsBackend.globalFonts.defaultFont);
    lobbyButton->stretchWithAspectRatio = true;
    lobbyButton->moveWithAspectRatio = true;
    lobbyButton->centerText = true;
    lobbyButton->SetText("Lobby");
    lobbyButton->font.fontScale = 2.0;
    lobbyButton->scale = glm::vec2(0.2, 0.09);
    lobbyButton->position.y = -0.3f;
    lobbyButton->color.value = glm::vec4(0.2);
    lobbyButton->borderColor.value = glm::vec4(0.4);
    lobbyButton->cornerColor.value = glm::vec4(0.7);
    lobbyButton->onPressed = [this, &app]{
        #ifdef __EMSCRIPTEN__
        if(app->networkManager.IsConnected()) {
            std::shared_ptr<LobbyWidgetLayer> lobbyLayer = app->sceneManager.currentScene->GetWidgetLayerByType<LobbyWidgetLayer>();
            lobbyLayer->invisible = !lobbyLayer->invisible;
            lobbyLayer->justEnabled = true;
            disabled = true;
        }
        #else
        std::shared_ptr<LobbyWidgetLayer> lobbyLayer = app->sceneManager.currentScene->GetWidgetLayerByType<LobbyWidgetLayer>();
        lobbyLayer->invisible = !lobbyLayer->invisible;
        lobbyLayer->justEnabled = true;
        disabled = true;
        #endif
    };

    std::shared_ptr<TextRectWidget> rect = CreateWidget<TextRectWidget>("rect", app->graphicsBackend.globalFonts.defaultFont);
    rect->SetText("Welcome to the Fox2.io\n"
                "multiplayer test!\n\n"
                "Follow development at:\n"
                "- YouTube: @thelemon9300\n"
                "- X: @MichaelTeschner7");
    rect->position = glm::vec2(-0.7, 0.2);
    rect->moveWithAspectRatio = true;
    rect->scale = glm::vec2(0.4, 0.2);
    rect->color.value = glm::vec4(0.2);
    rect->borderColor.value = glm::vec4(0.4);
    rect->cornerColor.value = glm::vec4(0.7);

    connectionStatus = CreateWidget<TextRectWidget>("connectionStatus", app->graphicsBackend.globalFonts.defaultFont);
    connectionStatus->SetText("no server connection");
    connectionStatus->position = glm::vec2(0.8, -1.0);
    connectionStatus->moveWithAspectRatio = true;
    connectionStatus->scale = glm::vec2(0.4, 0.3);
    connectionStatus->color.value = glm::vec4(0.0);
    connectionStatus->borderColor.value = glm::vec4(0.0);
    connectionStatus->cornerColor.value = glm::vec4(0.0);
    connectionStatusId = connectionStatus->id;
}

void MenuWidgetLayer::UpdateLayer() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    if(connectionStatus) {
        std::static_pointer_cast<TextRectWidget>(connectionStatus)->SetText(app->networkManager.IsConnected() ? "connected\nlobby id: " + std::to_string(app->networkManager.GetLobbyId()) : "no server connection");
    }

    if(!invisible) {
        app->sceneManager.activeCamera.position = MathUtils::RotatePointAroundPoint(app->sceneManager.activeCamera.position, app->sceneManager.activeCamera.target, 0.1f * (float)app->clock.deltaTime, GLOBAL_UP);
    }
}

void LobbyWidgetLayer::CreateWidgets() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<RectWidget> background = CreateWidget<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 0.8);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->z_distance = -0.2f;

    std::shared_ptr<RectWidget> panel = CreateWidget<RectWidget>("panel");
    panel->moveWithAspectRatio = true;
    panel->stretchWithAspectRatio = true;
    panel->scale = glm::vec2(0.35f, 0.45f);
    panel->position = glm::vec2(0.0, -0.1f);
    panel->color.value = glm::vec4(0.1, 0.1, 0.1, 1.0f);
    panel->borderColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    panel->z_distance = -0.1f;

    std::shared_ptr<TextButtonWidget> closeButton = CreateWidget<TextButtonWidget>("closeButton", app->graphicsBackend.globalFonts.defaultFont);
    closeButton->moveWithAspectRatio = true;
    closeButton->centerText = true;
    closeButton->SetText("X");
    closeButton->font.fontScale = 1.0f;
    closeButton->scale = glm::vec2(0.04);
    closeButton->position = glm::vec2(0.28f, 0.25f);
    closeButton->color.value = glm::vec4(0.2);
    closeButton->borderColor.value = glm::vec4(0.4);
    closeButton->cornerColor.value = glm::vec4(0.4);
    closeButton->z_distance = -0.1f;
    closeButton->onPressed = [this, &app] {
        std::shared_ptr<MenuWidgetLayer> menuLayer = app->sceneManager.currentScene->GetWidgetLayerByType<MenuWidgetLayer>();
        menuLayer->disabled = false;
        invisible = true;
    };

    codeInput = CreateWidget<InputWidget>("codeInput", app->graphicsBackend.globalFonts.defaultFont);
    codeInput->stretchWithAspectRatio = true;
    codeInput->moveWithAspectRatio = true;
    codeInput->centerText = true;
    codeInput->maxCharacters = 6;
    codeInput->SetText("******");
    codeInput->font.fontScale = 2.0;
    codeInput->scale = glm::vec2(0.2, 0.09);
    codeInput->position.y = 0.1f;
    codeInput->color.value = glm::vec4(0.2);
    codeInput->borderColor.value = glm::vec4(0.4);
    codeInput->cornerColor.value = glm::vec4(0.7);
    codeInput->z_distance = -0.1f;

    std::shared_ptr<TextButtonWidget> joinButton = CreateWidget<TextButtonWidget>("joinButton", app->graphicsBackend.globalFonts.defaultFont);
    joinButton->stretchWithAspectRatio = true;
    joinButton->moveWithAspectRatio = true;
    joinButton->centerText = true;
    joinButton->SetText("Join");
    joinButton->font.fontScale = 2.0;
    joinButton->scale = glm::vec2(0.2, 0.09);
    joinButton->position.y = -0.1f;
    joinButton->color.value = glm::vec4(0.2);
    joinButton->borderColor.value = glm::vec4(0.4);
    joinButton->cornerColor.value = glm::vec4(0.7);
    joinButton->onPressed = [this, &app]{
        uint32_t lobbyId = 0;
        try {
            if((lobbyId = (uint32_t)std::stoi(this->codeInput->GetText())) != 0) {
                app->networkManager.JoinLobby(lobbyId);
            }
        }
        catch (const std::exception& e) {
            std::cout << "Lobby code is invalid" << std::endl;
        }
    };
    joinButton->z_distance = -0.1f;

    std::shared_ptr<TextButtonWidget> createButton = CreateWidget<TextButtonWidget>("createButton", app->graphicsBackend.globalFonts.defaultFont);
    createButton->stretchWithAspectRatio = true;
    createButton->moveWithAspectRatio = true;
    createButton->centerText = true;
    createButton->SetText("Create");
    createButton->font.fontScale = 2.0;
    createButton->scale = glm::vec2(0.2, 0.09);
    createButton->position.y = -0.3f;
    createButton->color.value = glm::vec4(0.2);
    createButton->borderColor.value = glm::vec4(0.4);
    createButton->cornerColor.value = glm::vec4(0.7);
    createButton->onPressed = [this, &app]{
        app->networkManager.CreateLobby();
    };
    createButton->z_distance = -0.1f;
}

void SettingsWidgetLayer::CreateWidgets() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<RectWidget> background = CreateWidget<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 0.7);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->z_distance = -0.5f;

    std::shared_ptr<TextRectWidget> infoRect = CreateWidget<TextRectWidget>("infoRect", app->graphicsBackend.globalFonts.defaultFont);
    infoRect->SetText(
                "Info:\n"
                "- Get Heat Seeker lock by\n pointing nose at target\n"
                "- Get Radar Seeker lock by\n looking at target\n"
    );
    infoRect->position = glm::vec2(-0.3, 0.0);
    infoRect->moveWithAspectRatio = true;
    infoRect->scale = glm::vec2(0.4, 0.25);
    infoRect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    infoRect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    infoRect->z_distance = -0.5f;

    std::shared_ptr<TextRectWidget> settingsRect = CreateWidget<TextRectWidget>("settingsRect", app->graphicsBackend.globalFonts.defaultFont);
    settingsRect->SetText(
                "Controls:\n"
                "- Thottle Up: L-Shift\n"
                "- Thottle Down: L-Ctrl\n"
                "- Toggle Radar Lock: T\n"
                "- Free Look: C\n"
                "- Air Brake: V\n"
                "- Settings: L-Alt\n"
    );
    settingsRect->position = glm::vec2(0.3, 0.0);
    settingsRect->moveWithAspectRatio = true;
    settingsRect->scale = glm::vec2(0.4, 0.25);
    settingsRect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    settingsRect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    settingsRect->z_distance = -0.5f;
}

void SettingsWidgetLayer::UpdateLayer() {
    if(InputManager::IsKeyJustPressed(GLFW_KEY_LEFT_ALT)) {
        invisible = !invisible;
    }
}
