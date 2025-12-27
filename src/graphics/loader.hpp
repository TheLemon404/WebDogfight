#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "backend.hpp"

using json = nlohmann::json;

class Loader{
    static std::vector<unsigned int> GetIndicesFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data) {
        std::vector<unsigned int> result;

        unsigned int buffViewInd = accessor.value("bufferView", 1);
        unsigned int count = accessor["count"];
        unsigned int accByteOffset = accessor.value("byteOffset", 0);
        unsigned int componentType = accessor["componentType"];

        json bufferView = jsonData["bufferViews"][buffViewInd];
        unsigned int byteOffset = bufferView["byteOffset"];

        unsigned int beginningOfData = byteOffset + accByteOffset;

        if(componentType == 5125) {
            for(unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 4) {
                unsigned int value;
                std::memcpy(&value, &data[i], sizeof(unsigned int));
                result.push_back((unsigned int)value);
            }
        }
        else if(componentType == 5123) {
            for(unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2) {
                unsigned short value;
                std::memcpy(&value, &data[i], sizeof(unsigned short));
                result.push_back((unsigned int)value);
            }
        }
        else if(componentType == 5122) {
            for(unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2) {
                short value;
                std::memcpy(&value, &data[i], sizeof(short));
                result.push_back((unsigned int)value);
            }
        }

        return result;
    }

    static std::vector<float> GetFloatsFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data) {
        std::vector<float> result;

        unsigned int buffViewInd = accessor.value("bufferView", 1);
        unsigned int count = accessor["count"];
        unsigned int accByteOffset = accessor.value("byteOffset", 0);
        std::string type = accessor["type"];

        json bufferView = jsonData["bufferViews"][buffViewInd];
        unsigned int byteOffset = bufferView["byteOffset"];

        unsigned int numPerVert;
        if (type == "SCALAR") numPerVert = 1;
        else if (type == "VEC2") numPerVert = 2;
        else if (type == "VEC3") numPerVert = 3;
        else if (type == "VEC4") numPerVert = 4;
        else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4");

        unsigned int beginningOfData = byteOffset + accByteOffset;
        unsigned int lengthOfData = count * 4 * numPerVert;
        for(unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
        {
            float value;
            std::memcpy(&value, &data[i], sizeof(float));
            result.push_back(value);
        }

        return result;
    }

    static std::vector<glm::vec3> AssembleFloatsToVec3(std::vector<float> data) {
        std::vector<glm::vec3> result;
        for(size_t i = 0; i < data.size(); i) {
            result.push_back({
                data[i++],
                data[i++],
                data[i++]
            });
        }
        return result;
    }

    static std::vector<glm::vec2> AssembleFloatsToVec2(std::vector<float> data) {
        std::vector<glm::vec2> result;
        for(size_t i = 0; i < data.size(); i) {
            result.push_back({
                data[i++],
                data[i++]
            });
        }
        return result;
    }

    public:
    static Mesh LoadMeshFromGLTF(const char* resourcePath) {
        std::string text = Files::ReadResource(resourcePath);
        json JSON = json::parse(text);

        //load the gltf file binary
        std::string uri = JSON["buffers"][0]["uri"];
        std::string fileStr = std::string(resourcePath);
        std::string binaryFileDirectory = fileStr.substr(0, fileStr.find_last_of("/") + 1);
        std::cout << "attemping to read GLTF binary file at: " << binaryFileDirectory + uri << std::endl;
        std::vector<unsigned char> data = Files::ReadResourceBytes(binaryFileDirectory + uri);

        std::cout << "File Length: " << data.size() << std::endl;

        //get accessors
        unsigned int posAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["POSITION"];
        unsigned int normalAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["NORMAL"];
        unsigned int texAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"];
        unsigned int indAccInd = JSON["meshes"][0]["primitives"][0]["indices"];

        std::vector<float> posVector = GetFloatsFromJSON(JSON["accessors"][posAccInd], JSON, data);
        std::vector<float> normVector = GetFloatsFromJSON(JSON["accessors"][normalAccInd], JSON, data);
        std::vector<float> texVector = GetFloatsFromJSON(JSON["accessors"][texAccInd], JSON, data);
        std::vector<unsigned int> indices = GetIndicesFromJSON(JSON["accessors"][indAccInd], JSON, data);

        std::vector<glm::vec3> positions = AssembleFloatsToVec3(posVector);
        std::vector<glm::vec3> normals = AssembleFloatsToVec3(normVector);
        std::vector<glm::vec2> uvs = AssembleFloatsToVec2(texVector);

        std::vector<Vertex> vertices;
        for(size_t i = 0; i < positions.size(); i++) {
            vertices.push_back({
                positions[i],
                normals[i],
                uvs[i]
            });
        }

        Mesh mesh = Mesh(0, 0, 0, vertices.size(), indices.size());
        GraphicsBackend::UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
        return mesh;
    }
};
