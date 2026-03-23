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
};
