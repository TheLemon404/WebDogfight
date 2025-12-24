#pragma once

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>

class Files {
    public:
    static std::string ReadResource(const std::string resourcePath) {
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
};
