#pragma once

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

class Files {
    public:

    static std::string ReadResourceString(const std::string& resourcePath);
    static std::vector<unsigned char> ReadResourceBytes(const std::string& resourcePath);
};
