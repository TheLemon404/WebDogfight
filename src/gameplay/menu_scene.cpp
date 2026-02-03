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

    std::shared_ptr<TextButtonWidget> playButton = std::make_shared<TextButtonWidget>("playButton", GraphicsBackend::globalFonts.defaultFont);
    playButton->stretchWithAspectRatio = true;
    playButton->moveWithAspectRatio = true;
    playButton->centerText = true;
    playButton->SetText("Join");
    playButton->font.fontScale = 2.0;
    playButton->scale = glm::vec2(0.2, 0.09);
    playButton->position.y = -0.1f;
    playButton->color.value = glm::vec4(0.2);
    playButton->borderColor.value = glm::vec4(0.4);
    playButton->cornerColor.value = glm::vec4(0.7);
    playButton->onPressed = SceneManager::FutureChangeToGameScene;
    widgets.push_back(playButton);

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
