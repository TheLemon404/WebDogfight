#include "loader.hpp"
#include "backend.hpp"

#include "freetype/freetype.h"
#include "freetype/ftmodapi.h"
#include "types.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<unsigned int> Loader::GetIntsFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data) {
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
    else if(componentType == 5121) {
        for(unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 1) {
            uint8_t value;
            std::memcpy(&value, &data[i], sizeof(uint8_t));
            result.push_back((unsigned int)value);
        }
    }

    return result;
}

std::vector<float> Loader::GetFloatsFromJSON(json accessor, json jsonData, std::vector<unsigned char>& data) {
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
    else if (type == "MAT4") numPerVert = 16;
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

std::vector<glm::mat4> Loader::AssembleFloatsToMat4(std::vector<float> data) {
    std::vector<glm::mat4> result;
    for(size_t i = 0; i < data.size(); i) {
        result.push_back({
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
            data[i++],
        });
    }
    return result;
}

std::vector<glm::vec4> Loader::AssembleFloatsToVec4(std::vector<float> data) {
    std::vector<glm::vec4> result;
    for(size_t i = 0; i < data.size(); i) {
        result.push_back({
            data[i++],
            data[i++],
            data[i++],
            data[i++]
        });
    }
    return result;
}

std::vector<glm::ivec4> Loader::AssembleIntsToIVec4(std::vector<unsigned int> data) {
    std::vector<glm::ivec4> result;
    for(size_t i = 0; i < data.size(); i += 4) {
        result.push_back({
            data[i],
            data[i + 1],
            data[i + 2],
            data[i + 3]
        });
    }
    return result;
}

std::vector<glm::vec3> Loader::AssembleFloatsToVec3(std::vector<float> data) {
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

std::vector<glm::vec2> Loader::AssembleFloatsToVec2(std::vector<float> data) {
    std::vector<glm::vec2> result;
    for(size_t i = 0; i < data.size(); i) {
        result.push_back({
            data[i++],
            data[i++]
        });
    }
    return result;
}

void Loader::SplitShaderSource(const std::string& shaderSource, std::string& vertexSource, std::string& fragmentSource) {
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


Shader Loader::LoadShaderFromGLSL(const std::string& resourcePath) {
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

Mesh Loader::LoadMeshFromGLTF(const char* resourcePath) {
    std::string text = Files::ReadResourceString(resourcePath);
    json JSON = json::parse(text);

    //load the gltf file binary
    std::string uri = JSON["buffers"][0]["uri"];
    std::string fileStr = std::string(resourcePath);
    std::string binaryFileDirectory = fileStr.substr(0, fileStr.find_last_of("/") + 1);
    std::cout << "Attemping to read GLTF binary file at: " << binaryFileDirectory + uri << std::endl;
    std::vector<unsigned char> data = Files::ReadResourceBytes(binaryFileDirectory + uri);

    //get accessors
    unsigned int posAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indAccInd = JSON["meshes"][0]["primitives"][0]["indices"];

    std::vector<float> posVector = GetFloatsFromJSON(JSON["accessors"][posAccInd], JSON, data);
    std::vector<float> normVector = GetFloatsFromJSON(JSON["accessors"][normalAccInd], JSON, data);
    std::vector<float> texVector = GetFloatsFromJSON(JSON["accessors"][texAccInd], JSON, data);
    std::vector<unsigned int> indices = GetIntsFromJSON(JSON["accessors"][indAccInd], JSON, data);

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

SkeletalMesh Loader::LoadSkeletalMeshFromGLTF(const char* resourcePath) {
    std::string text = Files::ReadResourceString(resourcePath);
    json JSON = json::parse(text);

    //load the gltf file binary
    std::string uri = JSON["buffers"][0]["uri"];
    std::string fileStr = std::string(resourcePath);
    std::string binaryFileDirectory = fileStr.substr(0, fileStr.find_last_of("/") + 1);
    std::cout << "Attemping to read GLTF binary file at: " << binaryFileDirectory + uri << std::endl;
    std::vector<unsigned char> data = Files::ReadResourceBytes(binaryFileDirectory + uri);

    //construct skeleton
    Skeleton skeleton;

    //construct the bones
    int numJoints = JSON["skins"][0]["joints"].size();
    skeleton.bones.resize(numJoints);
    for(size_t i = 0; i < numJoints; i++){
        int jointIndex = JSON["skins"][0]["joints"][i];
        Bone bone = Bone();
        bone.name = JSON["nodes"][jointIndex]["name"];
        if(JSON["nodes"][jointIndex].contains("translation"))
        {
            bone.position = {JSON["nodes"][jointIndex]["translation"][0], JSON["nodes"][jointIndex]["translation"][1], JSON["nodes"][jointIndex]["translation"][2]};
        }
        if(JSON["nodes"][jointIndex].contains("rotation")){
            bone.rotation = glm::quat(
                JSON["nodes"][jointIndex]["rotation"][3],
                JSON["nodes"][jointIndex]["rotation"][0],
                JSON["nodes"][jointIndex]["rotation"][1],
                JSON["nodes"][jointIndex]["rotation"][2]
            );
            //store this base pose for later
            bone.restingRotation = glm::quat(
                JSON["nodes"][jointIndex]["rotation"][3],
                JSON["nodes"][jointIndex]["rotation"][0],
                JSON["nodes"][jointIndex]["rotation"][1],
                JSON["nodes"][jointIndex]["rotation"][2]
            );
        }
        if(JSON["nodes"][jointIndex].contains("scale"))
        {
            bone.scale = {JSON["nodes"][jointIndex]["scale"][0], JSON["nodes"][jointIndex]["scale"][1], JSON["nodes"][jointIndex]["scale"][2]};
        }
        bone.id = jointIndex;
        skeleton.bones[jointIndex] = bone;
    }

    //parent the bones properly
    for(size_t i = 0; i < numJoints; i++){
        if(JSON["nodes"][i].contains("children")) {
            for(size_t j = 0; j < JSON["nodes"][i]["children"].size(); j++) {
                int childIndex = JSON["nodes"][i]["children"][j];
                skeleton.bones[childIndex].parentID = skeleton.bones[i].id;
            }
        }
    }

    //get accessors
    unsigned int posAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int invViewMatInd = JSON["skins"][0]["inverseBindMatrices"];
    unsigned int jointsAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["JOINTS_0"];
    unsigned int weightsAccInd = JSON["meshes"][0]["primitives"][0]["attributes"]["WEIGHTS_0"];
    unsigned int indAccInd = JSON["meshes"][0]["primitives"][0]["indices"];

    std::vector<float> posVector = GetFloatsFromJSON(JSON["accessors"][posAccInd], JSON, data);
    std::vector<float> normVector = GetFloatsFromJSON(JSON["accessors"][normalAccInd], JSON, data);
    std::vector<float> texVector = GetFloatsFromJSON(JSON["accessors"][texAccInd], JSON, data);
    std::vector<float> invBindMatVector = GetFloatsFromJSON(JSON["accessors"][invViewMatInd], JSON, data);
    std::vector<unsigned int> jointsVector = GetIntsFromJSON(JSON["accessors"][jointsAccInd], JSON, data);
    std::vector<float> weightsVector = GetFloatsFromJSON(JSON["accessors"][weightsAccInd], JSON, data);
    std::vector<unsigned int> indices = GetIntsFromJSON(JSON["accessors"][indAccInd], JSON, data);

    std::vector<glm::vec3> positions = AssembleFloatsToVec3(posVector);
    std::vector<glm::vec3> normals = AssembleFloatsToVec3(normVector);
    std::vector<glm::vec2> uvs = AssembleFloatsToVec2(texVector);
    std::vector<glm::mat4> invBindMatrices = AssembleFloatsToMat4(invBindMatVector);
    std::vector<glm::ivec4> joints = AssembleIntsToIVec4(jointsVector);
    std::vector<glm::vec4> weights = AssembleFloatsToVec4(weightsVector);

    for(size_t i = 0; i < numJoints; i++) {
        skeleton.bones[JSON["skins"][0]["joints"][i]].inverseBindMatrix = invBindMatrices[i];
    }

    std::vector<Vertex> vertices;
    int last = 0;
    for(size_t i = 0; i < positions.size(); i++) {
        vertices.push_back({
            positions[i],
            normals[i],
            uvs[i],
            //we only need the first index, since each vertex is only effected by one bone
            (unsigned int)JSON["skins"][0]["joints"][joints[i].x]
        });
    }

    SkeletalMesh mesh = SkeletalMesh(0, 0, 0, vertices.size(), indices.size());
    mesh.skeleton = skeleton;
    GraphicsBackend::UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
    return mesh;
}

Texture Loader::LoadTextureFromFile(const char* resourcePath) {
    Texture texture = Texture();

    std::cout << "Attemping to read Texture file at: " << resourcePath << std::endl;

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *data = stbi_load(resourcePath, &texture.width, &texture.height, &texture.channels, 0);
    if(data) {
        if(texture.channels == 1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture.width, texture.height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if(texture.channels == 2) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, texture.width, texture.height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if(texture.channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if(texture.channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8I, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        texture.data = data;
    }
    else {
        std::cout << "Failed to loat texture resource from file: " << resourcePath << std::endl;
    }

    return texture;
}

void Loader::LoadFontFromTTF(const char *resourcePath, Font& font) {
    std::cout << "Attemping to font file at: " << resourcePath << std::endl;

    if (FT_Init_FreeType(&font.freetypeLibrary))
    {
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }

    if (FT_New_Face(font.freetypeLibrary, resourcePath, 0, &font.freetypeFace))
    {
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    }
    FT_Set_Pixel_Sizes(font.freetypeFace, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(unsigned char c = 0; c < 128; c++){
        if(FT_Load_Char(font.freetypeFace, c, FT_LOAD_RENDER)) {
            std::cout << "Warning: failed to load glyph: " << c << std::endl;
            continue;
        }

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, font.freetypeFace->glyph->bitmap.width, font.freetypeFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, font.freetypeFace->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = Character();
        character.textureID = textureID;
        character.size = glm::ivec2(font.freetypeFace->glyph->bitmap.width, font.freetypeFace->glyph->bitmap.rows);
        character.bearing = glm::ivec2(font.freetypeFace->glyph->bitmap_left, font.freetypeFace->glyph->bitmap_top);
        character.advance = font.freetypeFace->glyph->advance.x;
        font.characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(font.freetypeFace);
    FT_Done_Library(font.freetypeLibrary);
}
