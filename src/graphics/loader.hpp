#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "backend.hpp"
#include "freetype/freetype.h"
#include "glm/fwd.hpp"
#include "types.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

using json = nlohmann::json;

class Loader{
    static std::vector<unsigned int> GetIntsFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data);
    static std::vector<float> GetFloatsFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data);
    static std::vector<glm::mat4> AssembleFloatsToMat4(std::vector<float> data);
    static std::vector<glm::vec4> AssembleFloatsToVec4(std::vector<float> data);
    static std::vector<glm::ivec4> AssembleIntsToIVec4(std::vector<unsigned int> data);
    static std::vector<glm::vec3> AssembleFloatsToVec3(std::vector<float> data);
    static std::vector<glm::vec2> AssembleFloatsToVec2(std::vector<float> data);

    static void SplitShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource);

    inline static FT_Library freetypeLibrary;

    public:
    static Shader LoadShaderFromGLSL(const std::string& resourcePath);
    static Mesh LoadMeshFromGLTF(const char* resourcePath);
    static SkeletalMesh LoadSkeletalMeshFromGLTF(const char* resourcePath);
    static Texture LoadTextureFromFile(const char* resourcePath);
    static void LoadFontFromTTF(const char* resourcePath, Font& font);
};
