#include "application.hpp"
#include "GLFW/glfw3.h"

std::unique_ptr<Application> Application::instance = nullptr;
std::mutex Application::applicationInstanceMutex;

std::unique_ptr<Application>& Application::GetInstance() {
    std::lock_guard<std::mutex> lock(Application::applicationInstanceMutex);
    if (!instance) {
        instance = std::make_unique<Application>();
    }
    return instance;
}

bool Application::IsVisible() {
    #ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return document.hidden ? 0 : 1; });
    #else
    return !glfwGetWindowAttrib(windowManager.primaryWindow->window, GLFW_ICONIFIED);
    #endif
}
