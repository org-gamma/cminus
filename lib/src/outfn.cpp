
#include "../outfn.hpp"

#include <iostream>
#include <cstdio>
#include <cstdarg>

namespace cminus {

    void print(const std::string& text) {
        std::cout << text;
    }

    void println(const std::string& text) {
        std::cout << text << std::endl;
    }

    void printf(const char* format, ...) {
        va_list args;

        va_start(args, format);
        std::vprintf(format, args);
        va_end(args);
    }

}
