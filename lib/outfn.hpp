
#ifndef CMINUS_OUTFN_HPP
#define CMINUS_OUTFN_HPP

#include <string>
#include <cstdarg>

namespace cminus {

    // Prints text without a newline.
    void print(const std::string& text);

    // Prints text followed by a newline.
    void println(const std::string& text);

    // Formatted output, similar to C's printf.
    void printf(const char* format, ...);

}

#endif
