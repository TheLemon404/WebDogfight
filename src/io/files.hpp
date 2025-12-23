#pragma once

#include <cstdio>
#include <stdexcept>
#include <string>

class Files {
    public:
    static std::string ReadResource(const std::string& resourcePath) {
        FILE* file = std::fopen(resourcePath.c_str(), "r");
        if (!file) {
            throw std::runtime_error("Failed to open file at resource path: " + resourcePath);
        }
        std::string content;
        char c;
        while((c = fgetc(file)) != EOF) {
            content += c;
        }
        std::fclose(file);
        return content;
    }
};
