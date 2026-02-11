#include "window.hpp"

#ifdef __EMSCRIPTEN__
EM_JS(int, html_get_width, (), {
    return window.innerWidth;
})

EM_JS(int, html_get_height, (), {
    return window.innerHeight;
})
#endif

#ifdef __EMSCRIPTEN__
EM_BOOL Window::EmscriptenResizeCallback(int event_type, const EmscriptenUiEvent *event, void *user_data) {
    Window* userWindow = static_cast<Window*>(user_data);
    if(userWindow){
        userWindow->width = html_get_width();
        userWindow->height = html_get_height();
        userWindow->aspect = (float)userWindow->width / userWindow->height;
        return 1;
    }
    return 0;
}
#else
void Window::GLFWResizeCallback(GLFWwindow* window, int width, int height) {
    Window* userWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(userWindow){
        userWindow->width = width;
        userWindow->height = height;
        userWindow->aspect = (float)userWindow->width / userWindow->height;
    }
    else {
        throw std::runtime_error("Failed to get user pointer from window");
    }
}
#endif

Window::Window() {
    #ifdef __EMSCRIPTEN__
        width = html_get_width();
        height = html_get_height();
        aspect = static_cast<float>(width)/height;
    #else
        width = 1600;
        height = 1000;
        aspect = static_cast<float>(1600)/1000;
    #endif
}

void Window::Open() {
    if(!glfwInit()) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW context");
    }

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwSetWindowUserPointer(window, this);

#ifdef __EMSCRIPTEN__
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, Window::EmscriptenResizeCallback);
#else
    glfwSetFramebufferSizeCallback(window, Window::GLFWResizeCallback);
#endif

    glfwSetScrollCallback(window, InputManager::GLFWMouseScrollCallback);
    glfwSetCursorPosCallback(window, InputManager::GLFWMouseCursorPosCallback);
    glfwSetMouseButtonCallback(window, InputManager::GLFWMouseButtonCallback);
    glfwSetKeyCallback(window, InputManager::GLFWKeyCallback);
    glfwSetCharCallback(window, InputManager::GLFWCharCallback);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
    gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);
#endif

    glfwSetInputMode(window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
