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
    Shader skybox;
    Shader terrain;
    Shader water;
    Shader clouds;
    Shader uiCircle;
    Shader uiSquare;
    Shader radar;
    Shader compass;
    Shader aircraft;
};

struct GlobalMeshes {
    Mesh FA_XX;
};

struct GlobalFonts {
    Font defaultFont;
};

struct GlobalTextures {
    Texture3D noiseTexture3D;
};

class GraphicsBackend {
    Mesh debugCube;
    Shader debugShader;

    int GetUniformLocation(Shader& shader, const std::string& var) {
        GLint location = shader.GetCachedUniformLocation(var);
        if(location == -1) {
            location = glGetUniformLocation(shader.programID, var.c_str());
            shader.SetCachedUniformLocation(var, location);
        }
        return location;
    }

    public:
    bool debugMode = false;

    GlobalShaders globalShaders;
    GlobalMeshes globalMeshes;
    GlobalFonts globalFonts;
    GlobalTextures globalTextures;

    glm::vec3 clearColor = glm::vec3(1.0f);

    void LoadResources();
    Mesh CreateCube();
    Mesh CreateQuad();

    void UpdateMeshVertices(Mesh& mesh, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices);

    void UploadMeshData(unsigned int& vao, unsigned int& vbo, unsigned int& ebo, std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void UploadShaderUniformMat4(Shader& shader, const glm::mat4& matrix, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void UploadShaderUniformVec4(Shader& shader, const glm::vec4& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform4fv(location, 1, glm::value_ptr(vector));
    }

    void UploadShaderUniformVec3(Shader& shader, const glm::vec3& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform3fv(location, 1, glm::value_ptr(vector));
    }

    void UploadShaderUniformVec2(Shader& shader, const glm::vec2& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform2fv(location, 1, glm::value_ptr(vector));
    }

    void UploadShaderUniformIVec2(Shader& shader, const glm::ivec2& vector, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform2iv(location, 1, glm::value_ptr(vector));
    }

    void UploadShaderUniformFloat(Shader& shader, const float val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1f(location, val);
    }

    void UploadShaderUniformInt(Shader& shader, const int val, const std::string& var) {
        GLint location = GetUniformLocation(shader, var);
        glUniform1i(location, val);
    }

    void UseTextureSlot(const Texture& texture, unsigned int slot) {
        switch (slot) {
            case 0:
                glActiveTexture(GL_TEXTURE0);
                break;
            case 1:
                glActiveTexture(GL_TEXTURE1);
                break;
            case 2:
                glActiveTexture(GL_TEXTURE2);
                break;
            case 3:
                glActiveTexture(GL_TEXTURE3);
                break;
        }
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    void UseTexture3DSlot(const Texture3D& texture, unsigned int slot) {
        switch (slot) {
            case 0:
                glActiveTexture(GL_TEXTURE0);
                break;
            case 1:
                glActiveTexture(GL_TEXTURE1);
                break;
            case 2:
                glActiveTexture(GL_TEXTURE2);
                break;
            case 3:
                glActiveTexture(GL_TEXTURE3);
                break;
        }
        glBindTexture(GL_TEXTURE_3D, texture.id);
    }

    void UseTextureIDSlot(const unsigned int textureID, unsigned int slot) {
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

    void ResetTextureSlots() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SetBackfaceCulling(bool value) {
        value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    }

    void SetDepthMask(bool value) {
        value ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    }

    void SetDepthTest(bool value) {
        glDepthFunc(GL_LESS);
        value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    void DeleteMesh(Mesh& mesh) {
        for(auto pair : mesh.textureMap) {
            DeleteTexture(pair.second);
        }

        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }

    void DeleteSkeletalMesh(SkeletalMesh& mesh) {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
    }

    void DeleteShader(Shader& shader) {
        glDeleteProgram(shader.programID);
    }

    void DeleteTexture(Texture& texture) {
        glDeleteTextures(1, &texture.id);
        stbi_image_free(texture.data);
    }

    void DeleteTexture3D(Texture3D& texture) {
        glDeleteTextures(1, &texture.id);
        stbi_image_free(texture.data);
    }

    void DeleteFont(Font& font) {
        glDeleteTextures(1, &font.atlasTextureID);
    }

    void UnloadResources();

    void BeginDrawSkeletalMesh(SkeletalMesh& mesh, Shader& shader, Camera& camera, Transform& transform);
    void EndDrawSkeletalMesh(Mesh& mesh);
    void BeginDrawMesh(Mesh& mesh, Shader& shader, Camera& camera, Transform& transform, bool hasTransform = true, bool ignoreDefaultMaterialProps = false);
    void EndDrawMesh(Mesh& mesh);
    void BeginDrawMeshInstanced(Mesh& mesh, Shader& shader, Camera& camera, Transform* transforms, size_t numParticles);
    void EndDrawMeshInstanced(Mesh& mesh, size_t numParticles);

    void BeginDrawMesh2D(Mesh& mesh, Shader& shader, glm::vec2& screenPosition, glm::vec2& scale, float rotation, float z_distance = -1.0f, bool stretchWithAspectRatio = false, bool moveWithAspectRatio = false);
    void EndDrawMesh2D(Mesh& mesh);

    void ResetState(int viewportWidth, int viewportHeight) {
       	glViewport(0, 0, viewportWidth, viewportHeight);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DrawSkybox(Skybox& skybox, Camera& camera);

    //debug drawing
    void DrawDebugCube(Camera& camera, Color color, Transform& transform);
};
