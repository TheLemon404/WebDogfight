#pragma once

#include <string>

class MiscUtils {
    public:

    static std::string Truncate(std::string str, int n) {
        if(str.length() < n)
        {
            return str;
        }

        return str.erase(str.length() - n, n);
    }

    static std::vector<std::string> Split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        tokens.push_back(str.substr(start));
        return tokens;
    }
};
