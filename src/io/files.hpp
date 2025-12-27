#pragma once

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

class Files {
    public:
    static std::string ReadResource(const std::string& resourcePath) {
        std::ifstream file(resourcePath);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + resourcePath);
        }
        std::string result = std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        return result;
    }

    static std::vector<unsigned char> ReadResourceBytes(const std::string& resourcePath) {
        std::ifstream file(resourcePath, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + resourcePath);
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);

        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw std::runtime_error("Error reading file: " + resourcePath);
        }

        return buffer;
    }
};
