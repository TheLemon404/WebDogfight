#include "backend.hpp"
#include "types.hpp"

void GraphicsBackend::LoadResources() {
    debugCube = CreateCube();
    debugShader = CreateShader("resources/shaders/flat.glsl");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsBackend::UnloadResources() {
    DeleteMesh(debugCube);
    DeleteShader(debugShader);
}

void GraphicsBackend::SplitShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource) {
    enum class Mode { NONE, VERTEX, FRAGMENT };
    std::istringstream stream(shaderSource);
    std::string line;
    Mode mode = Mode::NONE;

    while (std::getline(stream, line)) {
        // Strip CR (Windows)
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        // Strip UTF-8 BOM
        if (line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF) {
            line.erase(0, 3);
        }

        if (line == "#vertex") {
            mode = Mode::VERTEX;
            continue;
        }
        if (line == "#fragment") {
            mode = Mode::FRAGMENT;
            continue;
        }

        if (mode == Mode::VERTEX)
            vertexSource += line + '\n';
        else if (mode == Mode::FRAGMENT)
            fragmentSource += line + '\n';
    }

    if (vertexSource.empty() || fragmentSource.empty()) {
        throw std::runtime_error(
            "Invalid shader source, missing #vertex or #fragment");
    }
}

Shader GraphicsBackend::CreateShader(const std::string& resourcePath) {
    std::string shaderSource = Files::ReadResourceString(resourcePath);
    std::string vertexSource = "";
    std::string fragmentSource = "";
    SplitShaderSource(shaderSource, vertexSource, fragmentSource);

    std::cout << "Attempting to create shader from file: " << resourcePath << std::endl;

    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int programID = glCreateProgram();
    char* vSource = vertexSource.data();
    char* fSource = fragmentSource.data();

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    glShaderSource(vertexShaderID, 1, &vSource, NULL);
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    glShaderSource(fragmentShaderID, 1, &fSource, NULL);
    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return Shader(programID);
}

Mesh GraphicsBackend::CreateQuad() {
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f,  0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{ 0.5f, -0.5f,  0.0}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{ 0.5f,  0.5f,  0.0}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 2
        {{-0.5f,  0.5f,  0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3
    };

    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return Mesh(vao, vbo, ebo, vertices.size(), indices.size());
}

Mesh GraphicsBackend::CreateCube() {
    // -- IMPORTANT -- these vertices were generated by Claude
    // Define the 8 unique vertices of a cube
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 2
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 4
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 6
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 7

        // Left face
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 8
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 9
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 10
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 11

        // Right face
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 13
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 14
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 15

        // Top face
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 17
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 19

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 21
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 22
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}  // 23
    };

    // Define indices for the 6 faces (2 triangles per face)
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        5, 4, 7,
        7, 6, 5,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return Mesh(vao, vbo, ebo, vertices.size(), indices.size());
}

void GraphicsBackend::UploadMeshData(unsigned int& vao, unsigned int& vbo, unsigned int& ebo, std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneID));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void GraphicsBackend::BeginDrawSkeletalMesh(SkeletalMesh& mesh, Shader& shader, Camera& camera, Transform& transform) {
    glUseProgram(shader.programID);

    glBindVertexArray(mesh.vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    //vertex uniforms
    UploadShaderUniformMat4(shader, camera.GetProjectionMatrix(), "uProjection");
    UploadShaderUniformMat4(shader, camera.GetViewMatrix(), "uView");
    UploadShaderUniformMat4(shader, transform.GetMatrix(), "uTransform");

    for(size_t i = 0; i < mesh.skeleton.bones.size(); i++) {
        UploadShaderUniformMat4(shader, mesh.skeleton.bones[i].GetGlobalTransform(mesh.skeleton.bones) * mesh.skeleton.bones[i].inverseBindMatrix, "uJointTransforms[" + std::to_string(mesh.skeleton.bones[i].id) + "]");
    }

    //fragment uniforms
    UploadShaderUniformVec3(shader, mesh.material.albedo, "uAlbedo");
    UploadShaderUniformVec3(shader, mesh.material.shadowColor, "uShadowColor");
}

void GraphicsBackend::EndDrawSkeletalMesh(Mesh& mesh) {
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GraphicsBackend::BeginDrawMesh(Mesh& mesh, Shader& shader, Camera& camera, Transform& transform) {
    glUseProgram(shader.programID);

    glBindVertexArray(mesh.vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //vertex uniforms
    UploadShaderUniformMat4(shader, camera.GetProjectionMatrix(), "uProjection");
    UploadShaderUniformMat4(shader, camera.GetViewMatrix(), "uView");
    UploadShaderUniformMat4(shader, transform.GetMatrix(), "uTransform");

    //fragment uniforms
    UploadShaderUniformVec3(shader, mesh.material.albedo, "uAlbedo");
    UploadShaderUniformVec3(shader, mesh.material.shadowColor, "uShadowColor");
}

void GraphicsBackend::EndDrawMesh(Mesh& mesh) {
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GraphicsBackend::BeginDrawMesh2D(Mesh &mesh, Shader &shader, Camera &camera, glm::vec2 &screenPosition, glm::vec2 &scale) {
    glUseProgram(shader.programID);

    glBindVertexArray(mesh.vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    Transform t = Transform();
    t.position.x = screenPosition.x;
    t.position.y = screenPosition.y;
    t.position.z = -1.0f;
    t.scale.x = scale.x;
    t.scale.y = scale.y;

    //vertex uniforms
    UploadShaderUniformMat4(shader, t.GetMatrix(), "uTransform");

    //fragment uniforms
    UploadShaderUniformVec3(shader, mesh.material.albedo, "uAlbedo");
    UploadShaderUniformVec3(shader, mesh.material.shadowColor, "uShadowColor");
}

void GraphicsBackend::EndDrawMesh2D(Mesh &mesh) {
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GraphicsBackend::DrawSkybox(Skybox &skybox, Camera& camera) {
    glUseProgram(skybox.shader.programID);

    glBindVertexArray(skybox.mesh.vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //vertex uniforms
    UploadShaderUniformMat4(skybox.shader, camera.GetProjectionMatrix(), "uProjection");
    UploadShaderUniformMat4(skybox.shader, glm::mat4(glm::mat3(camera.GetViewMatrix())), "uView");

    //fragment uniforms
    UploadShaderUniformVec3(skybox.shader, static_cast<glm::vec3>(skybox.horizonColor.value), "uHorizonColor");
    UploadShaderUniformVec3(skybox.shader, static_cast<glm::vec3>(skybox.skyColor.value), "uSkyColor");

    glDrawElements(GL_TRIANGLES, skybox.mesh.indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GraphicsBackend::DrawDebugCube(Camera& camera, Transform& transform) {
    BeginDrawMesh(debugCube, debugShader, camera, transform);
    EndDrawMesh(debugCube);
}
