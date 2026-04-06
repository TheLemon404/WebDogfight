#include "test_scene.hpp"
#include "GLFW/glfw3.h"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"
#include "../networking/network_manager.hpp"
#include "../application.hpp"

void MenuWidgetLayer::CreateWidgets() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<RectWidget> background = CreateWidget<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);

    nameInput = CreateWidget<InputWidget>("nameInput", app->graphicsBackend.globalFonts.defaultFont);
    nameInput->stretchWithAspectRatio = true;
    nameInput->moveWithAspectRatio = true;
    nameInput->centerText = true;
    nameInput->maxCharacters = 6;
    nameInput->SetText("name");
    nameInput->font.fontScale = 2.0;
    nameInput->scale = glm::vec2(0.2, 0.09);
    nameInput->position.y = 0.3f;
    nameInput->color.value = glm::vec4(0.2);
    nameInput->borderColor.value = glm::vec4(0.4);
    nameInput->cornerColor.value = glm::vec4(0.7);
    nameInput->onUnFocus = [this, &app] {
        if(app->networkManager.connected && app->networkManager.GetLobbyId() != -1) {
            app->networkManager.networkGameState.clientStates[app->networkManager.localClientId].name = nameInput->GetText();
        }
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
        if((lobbyId = (uint32_t)std::stoi(this->codeInput->GetText())) != 0) {
            app->networkManager.JoinLobby(lobbyId);
        }
    };

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

    std::shared_ptr<TextButtonWidget> playButton = CreateWidget<TextButtonWidget>("playButton", app->graphicsBackend.globalFonts.defaultFont);
    playButton->stretchWithAspectRatio = true;
    playButton->moveWithAspectRatio = true;
    playButton->centerText = true;
    playButton->SetText("Play");
    playButton->font.fontScale = 2.0;
    playButton->scale = glm::vec2(0.2, 0.09);
    playButton->position.y = -0.5f;
    playButton->color.value = glm::vec4(0.2);
    playButton->borderColor.value = glm::vec4(0.4);
    playButton->cornerColor.value = glm::vec4(0.7);
    playButton->onPressed = [this, &app]{
        InputManager::mouseHidden = true;
        glfwSetInputMode(app->windowManager.primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        std::shared_ptr<Aircraft> aircraft = std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json", app->networkManager.localClientId);
        app->sceneManager.currentScene->RuntimeSpawn(aircraft);
        this->SetDisabled(true);
    };

    std::shared_ptr<TextRectWidget> rect = CreateWidget<TextRectWidget>("rect", app->graphicsBackend.globalFonts.defaultFont);
    rect->SetText("Welcome to the Fox2.io\n"
                "flight controls test!\n\n"
                "This is in the very early\n"
                "stages of development.\n\n"
                "Follow development at:\n"
                "- YouTube: @thelemon9300\n"
                "- X: @MichaelTeschner7");
    rect->position = glm::vec2(-0.7, 0.2);
    rect->moveWithAspectRatio = true;
    rect->scale = glm::vec2(0.4, 0.3);
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
        std::static_pointer_cast<TextRectWidget>(connectionStatus)->SetText(app->networkManager.connected ? "connected\nlobby id: " + std::to_string(app->networkManager.GetLobbyId()) : "no server connection");
    }
}

void SettingsWidgetLayer::CreateWidgets() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<RectWidget> background = CreateWidget<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 0.7);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->z_distance = -0.5f;

    std::shared_ptr<TextRectWidget> rect = CreateWidget<TextRectWidget>("rect", app->graphicsBackend.globalFonts.defaultFont);
    rect->SetText(
                "Controls:\n"
                "- Shift: Thottle Up\n"
                "- Ctrl: Thottle Down\n"
                "- Q: Roll Left\n"
                "- E: Roll Right\n"
                "- Alt: Free Mouse\n"
                "- Tab: Free Look\n\n"
                "Notes:\n"
                "- Respawn on terrain\n"
                "  or boundary collision.");
    rect->position = glm::vec2(0.0, 0.0);
    rect->moveWithAspectRatio = true;
    rect->scale = glm::vec2(0.4, 0.35);
    rect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    rect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    rect->z_distance = -0.5f;
}

void SettingsWidgetLayer::UpdateLayer() {
    if(InputManager::IsKeyJustPressed(GLFW_KEY_LEFT_ALT)) {
        SetDisabled(!disabled);
    }
}
