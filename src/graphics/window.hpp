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

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void PollAndSwapBuffers() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    void Close() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};
