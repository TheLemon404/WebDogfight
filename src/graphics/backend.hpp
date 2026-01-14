#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "types.hpp"

#include <cstring>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

enum ShaderReadMode {
    VERTEX,
    FRAGMENT
};

class GraphicsBackend {
    inline static Mesh debugCube;
    inline static Shader debugShader;

    static void SplitShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource);

    static int GetUniformLocation(Shader& shader, const std::string& var) {
        GLint location = shader.GetCachedUniformLocation(var);
        if(location == -1) {
            location = glGetUniformLocation(shader.programID, var.c_str());
            shader.SetCachedUniformLocation(var, location);
        }
        return location;
    }

    public:
    inline static bool debugMode = false;

    static void LoadResources();
    static Shader CreateShader(const std::string& resourcePath);
    static Mesh CreateCube();
    static Mesh CreateQuad();
    static void UploadMeshData(unsigned int& vao, unsigned int& vbo, unsigned int& ebo, std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    static void UploadShaderUniformMat4(Shader& shader, const glm::mat4& matrix, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    static void UploadShaderUniformVec4(Shader& shader, const glm::vec4& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform4fv(location, 1, glm::value_ptr(vector));
    }

    static void UploadShaderUniformVec3(Shader& shader, const glm::vec3& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform3fv(location, 1, glm::value_ptr(vector));
    }

    static void UploadShaderUniformVec2(Shader& shader, const glm::vec2& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform2fv(location, 1, glm::value_ptr(vector));
    }

    static void UploadShaderUniformFloat(Shader& shader, const float val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1f(location, val);
    }

    static void UploadShaderUniformInt(Shader& shader, const int val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1i(location, val);
    }

    static void SetBackfaceCulling(bool value) {
        value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    }

    static void SetDepthMask(bool value) {
        value ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    }

    static void SetDepthTest(bool value) {
        glDepthFunc(GL_LESS);
        value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    static void DeleteMesh(Mesh& mesh) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }

    static void DeleteSkeletalMesh(SkeletalMesh& mesh) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }

    static void DeleteShader(Shader& shader) {
        glDeleteProgram(shader.programID);
    }

    static void UnloadResources();

    static void BeginDrawSkeletalMesh(SkeletalMesh& mesh, Shader& shader, Camera& camera, Transform& transform);
    static void EndDrawSkeletalMesh(Mesh& mesh);
    static void BeginDrawMesh(Mesh& mesh, Shader& shader, Camera& camera, Transform& transform);
    static void EndDrawMesh(Mesh& mesh);

    static void BeginDrawMesh2D(Mesh& mesh, Shader& shader, Camera& camera, glm::vec2& screenPosition, glm::vec2& scale, float rotation);
    static void EndDrawMesh2D(Mesh& mesh);

    static void ResetState(int viewportWidth, int viewportHeight) {
       	glViewport(0, 0, viewportWidth, viewportHeight);
        float val = 25.5 / 255.0;
        glClearColor(val, val, val, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void DrawSkybox(Skybox& skybox, Camera& camera);

    //debug drawing
    static void DrawDebugCube(Camera& camera, Transform& transform);
};
