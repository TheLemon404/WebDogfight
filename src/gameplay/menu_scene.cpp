#include "menu_scene.hpp"
#include "scene_manager.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"

void MenuWidgetLayer::CreateWidgets() {
    std::shared_ptr<RectWidget> background = std::make_shared<RectWidget>("background");
    background->color.value = glm::vec4(0.3, 0.3, 0.3, 1.0);
    background->stretchWithAspectRatio = true;
    background->borderColor.value = glm::vec4(0.4, 0.4, 0.4, 1.0);
    background->cornerColor.value = glm::vec4(0.4, 0.4, 0.4, 1.0);
    widgets.push_back(background);

    std::shared_ptr<InputWidget> codeInput = std::make_shared<InputWidget>("codeInput", GraphicsBackend::globalFonts.defaultFont);
    codeInput->stretchWithAspectRatio = true;
    codeInput->SetText("******");
    codeInput->font.fontScale = 2.0;
    codeInput->scale = glm::vec2(0.2, 0.09);
    codeInput->position.y = 0.1f;
    codeInput->color.value = glm::vec4(0.4);
    codeInput->borderColor.value = glm::vec4(0.5);
    codeInput->cornerColor.value = glm::vec4(0.7);
    widgets.push_back(codeInput);

    std::shared_ptr<TextButtonWidget> playButton = std::make_shared<TextButtonWidget>("playButton", GraphicsBackend::globalFonts.defaultFont);
    playButton->stretchWithAspectRatio = true;
    playButton->SetText("Join");
    playButton->font.fontScale = 2.0;
    playButton->scale = glm::vec2(0.2, 0.09);
    playButton->position.y = -0.1f;
    playButton->color.value = glm::vec4(0.4);
    playButton->borderColor.value = glm::vec4(0.5);
    playButton->cornerColor.value = glm::vec4(0.7);
    playButton->onPressed = SceneManager::FutureChangeToGameScene;
    widgets.push_back(playButton);
}
