#include "gameplay/scene.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>

//Vertex shader source code
const char* vertexShaderSource = "#version 300 es\n"
"precision mediump float;\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // Fixed: added semicolon and closing paren
"}\0";

const char* fragmentShaderSource = "#version 300 es\n"
"precision mediump float;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8, 0.3, 0.02, 1.0);\n"  // Fixed: added semicolon
"}\n\0";

struct ApplicationSettings {
    int screenWidth = 800;
    int screenHeight = 600;
};

class Application {
public:
    unsigned int shaderProgram;
    unsigned int vao;

    ApplicationSettings settings;
    Window window;
    std::unique_ptr<Scene> currentScene;

    bool Initialize() {
        currentScene->Initialize();

        return true;
    }

    void LoadAssets() {
        currentScene->LoadAssets();
    }

    void Update() {
        currentScene->Update();
    }

    void Draw() {
        currentScene->Draw();
    }

    void UnloadAssets() {
        currentScene->UnloadAssets();
    }
};

void main_loop(void* arg) {
    Application* app = static_cast<Application*>(arg);

    app->Update();
    app->Draw();

    float val = 25.0 / 255.0;

   	glViewport(0, 0, app->window.width, app->window.height);

    glClearColor(val, val, val, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(app->shaderProgram);
	glBindVertexArray(app->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);


    app->window.PollAndSwapBuffers();
}

int main() {
    Application app = Application();
    app.window = Window();
    app.window.width = app.settings.screenWidth;
    app.window.height = app.settings.screenHeight;
    app.window.title = "Fox2";

    app.currentScene = std::make_unique<TestScene>();

    if(app.Initialize()) {
        app.window.Open();
        app.LoadAssets();
    }

    GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
	};

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Create shader program
	app.shaderProgram = glCreateProgram();
	glAttachShader(app.shaderProgram, vertexShader);
	glAttachShader(app.shaderProgram, fragmentShader);
	glLinkProgram(app.shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Create VBO(Vertex Buffer Object)
	GLuint VBO;
	glGenVertexArrays(1, &app.vao);
	glBindVertexArray(app.vao);

	//Create VAO(Vertex Array Object)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, &app, 0, 1);
#else
    while(!app.window.ShouldClose()) {
        main_loop(&app);
    }
#endif

    glDeleteVertexArrays(1, &app.vao);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(app.shaderProgram);

    app.UnloadAssets();
    app.window.Close();

    return 0;
}
