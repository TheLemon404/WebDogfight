#include <GLFW/glfw3.h>
#include <stdexcept>

class Window {
private:
    int width, height;
    const char* title;
    GLFWwindow* window;

public:
    Window(int width, int height, const char* const title) {
        this->width = width;
        this->height = height;
        this->title = title;
    }

    void Open() {
        if(!glfwInit()) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW context");
        }

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void PollAndSwap() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Close() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};
