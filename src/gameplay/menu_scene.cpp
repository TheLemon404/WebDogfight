#include "menu_scene.hpp"
#include "GLFW/glfw3.h"
#include "scene_manager.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"

void MenuWidgetLayer::CreateWidgets() {
    std::shared_ptr<RectWidget> background = std::make_shared<RectWidget>("background");
    background->color.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    background->cornerColor.value = glm::vec4(0.1, 0.1, 0.1, 1.0);
    widgets.push_back(background);

    codeInput = std::make_shared<InputWidget>("codeInput", GraphicsBackend::globalFonts.defaultFont);
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
    widgets.push_back(codeInput);

    std::shared_ptr<TextButtonWidget> joinButton = std::make_shared<TextButtonWidget>("playButton", GraphicsBackend::globalFonts.defaultFont);
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
    joinButton->onPressed = SceneManager::FutureChangeToGameScene;
    widgets.push_back(joinButton);

    std::shared_ptr<TextButtonWidget> createButton = std::make_shared<TextButtonWidget>("playButton", GraphicsBackend::globalFonts.defaultFont);
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
    createButton->onPressed = SceneManager::FutureChangeToGameScene;
    widgets.push_back(createButton);

    std::shared_ptr<TextRectWidget> rect = std::make_shared<TextRectWidget>("rect", GraphicsBackend::globalFonts.defaultFont);
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
    widgets.push_back(rect);
}
