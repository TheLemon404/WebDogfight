#include "menu_scene.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"

void MenuWidgetLayer::CreateWidgets() {
    Font font = Font();
    Loader::LoadFontFromTTF("resources/fonts/JetBrainsMono-Medium.ttf", font);

    std::shared_ptr<InputWidget> codeInput = std::make_shared<InputWidget>("codeInput", font);
    codeInput->SetText("******");
    codeInput->font.fontScale = 2.0;
    codeInput->scale = glm::vec2(0.2, 0.09);
    codeInput->position.y = 0.1f;
    codeInput->color.value = glm::vec4(0.4);
    codeInput->borderColor.value = glm::vec4(0.5);
    codeInput->cornerColor.value = glm::vec4(0.7);
    widgets.push_back(codeInput);

    std::shared_ptr<TextButtonWidget> playButton = std::make_shared<TextButtonWidget>("playButton", font);
    playButton->SetText("Play");
    playButton->font.fontScale = 2.0;
    playButton->scale = glm::vec2(0.2, 0.09);
    playButton->position.y = -0.1f;
    playButton->color.value = glm::vec4(0.4);
    playButton->borderColor.value = glm::vec4(0.5);
    playButton->cornerColor.value = glm::vec4(0.7);
    widgets.push_back(playButton);

    //-- TODO -- add play-button and lobby code ui
}

void MenuWidgetLayer::UpdateLayer() {

}
