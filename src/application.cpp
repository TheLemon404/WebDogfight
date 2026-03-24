#include "application.hpp"

std::unique_ptr<Application> Application::instance = nullptr;
std::mutex Application::applicationInstanceMutex;

std::unique_ptr<Application>& Application::GetInstance() {
    std::lock_guard<std::mutex> lock(Application::applicationInstanceMutex);
    if (!instance) {
        instance = std::make_unique<Application>();
    }
    return instance;
}
