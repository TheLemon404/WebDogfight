#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "types.hpp"

#include <cstddef>
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

#include "stb_image.h"

enum ShaderReadMode {
    VERTEX,
    FRAGMENT
};

struct GlobalShaders {
    Shader flat;
    Shader font;
    Shader trails;
    Shader particles;
    Shader skeletal;
    Shader skybox;
    Shader terrain;
    Shader uiCircle;
    Shader uiSquare;
};

struct GlobalFonts {
    Font defaultFont;
};

class GraphicsBackend {
    inline static Mesh debugCube;
    inline static Shader debugShader;

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

    inline static GlobalShaders globalShaders;
    inline static GlobalFonts globalFonts;

    inline static glm::vec3 clearColor = glm::vec3(0.2);

    static void LoadResources();
    static Mesh CreateCube();
    static Mesh CreateQuad();

    static void UpdateMeshVertices(Mesh& mesh, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);

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

    static void UploadShaderUniformIVec2(Shader& shader, const glm::ivec2& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform2iv(location, 1, glm::value_ptr(vector));
    }

    static void UploadShaderUniformFloat(Shader& shader, const float val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1f(location, val);
    }

    static void UploadShaderUniformInt(Shader& shader, const int val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1i(location, val);
    }

    static void UseTextureSlot(const Texture& texture, unsigned int slot) {
        switch (slot) {
            case 0:
                glActiveTexture(GL_TEXTURE0);
                break;
            case 1:
                glActiveTexture(GL_TEXTURE1);
                break;
        }
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    static void UseTextureIDSlot(const unsigned int textureID, unsigned int slot) {
        switch (slot) {
            case 0:
                glActiveTexture(GL_TEXTURE0);
                break;
            case 1:
                glActiveTexture(GL_TEXTURE1);
                break;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    static void ResetTextureSlots() {
        glBindTexture(GL_TEXTURE_2D, 0);
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

    static void DeleteTexture(Texture& texture) {
        glDeleteTextures(1, &texture.id);
        stbi_image_free(texture.data);
    }

    static void DeleteFont(Font& font) {
        glDeleteTextures(1, &font.atlasTextureID);
    }

    static void UnloadResources();

    static void BeginDrawSkeletalMesh(SkeletalMesh& mesh, Shader& shader, Camera& camera, Transform& transform);
    static void EndDrawSkeletalMesh(Mesh& mesh);
    static void BeginDrawMesh(Mesh& mesh, Shader& shader, Camera& camera, Transform& transform, bool hasTransform = true);
    static void EndDrawMesh(Mesh& mesh);
    static void BeginDrawMeshInstanced(Mesh& mesh, Shader& shader, Camera& camera, Transform* transforms, size_t numParticles);
    static void EndDrawMeshInstanced(Mesh& mesh, size_t numParticles);

    static void BeginDrawMesh2D(Mesh& mesh, Shader& shader, glm::vec2& screenPosition, glm::vec2& scale, float rotation, bool stretchWithAspectRatio = false, bool moveWithAspectRatio = false);
    static void EndDrawMesh2D(Mesh& mesh);

    static void ResetState(int viewportWidth, int viewportHeight) {
       	glViewport(0, 0, viewportWidth, viewportHeight);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void DrawSkybox(Skybox& skybox, Camera& camera);

    //debug drawing
    static void DrawDebugCube(Camera& camera, Transform& transform);
};
