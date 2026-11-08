
#include "lexer.hpp"
#include "parser.hpp"
#include "transpiler.hpp"
#include "errors.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace {

    void printUsage() {
        std::cout
            << "CMinus Compiler\n"
            << "Usage: cminus <input.cm> [-o output.cpp]\n";
    }

    std::string getDefaultOutput(const std::string& input) {
        size_t extension = input.find_last_of('.');

        if (extension == std::string::npos)
            return input + ".cpp";

        return input.substr(0, extension) + ".cpp";
    }

    std::string readFile(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not open input file: " + filename
            );
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }

    void writeFile(
        const std::string& filename,
        const std::string& content
    ) {
        std::ofstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not create output file: " + filename
            );
        }

        file << content;

        if (!file.good()) {
            throw std::runtime_error(
                "Failed to write output file: " + filename
            );
        }
    }

}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile;

    // =========================
    // ARGUMENT PARSING
    // =========================

    for (int i = 2; i < argc; i++) {

        std::string argument = argv[i];

        if (argument == "-o") {

            if (i + 1 >= argc) {
                std::cerr
                    << "CMinus Error: Missing output filename.\n";

                return 1;
            }

            outputFile = argv[++i];
        }
        else if (argument == "--help" || argument == "-h") {
            printUsage();
            return 0;
        }
        else {
            std::cerr
                << "CMinus Error: Unknown argument: "
                << argument << '\n';

            return 1;
        }
    }

    if (outputFile.empty()) {
        outputFile = getDefaultOutput(inputFile);
    }

    // =========================
    // COMPILATION
    // =========================

    try {

        std::cout << "CMinus >> Compiling "
                  << inputFile << "...\n";

        // Read source
        std::string source = readFile(inputFile);

        // Lexical analysis
        std::cout << "CMinus >> Lexing...\n";

        cminus::Lexer lexer(source);

        std::vector<cminus::Token> tokens =
            lexer.tokenize();

        // Parsing
        std::cout << "CMinus >> Parsing...\n";

        cminus::Parser parser(tokens);

        std::unique_ptr<cminus::Program> program =
            parser.parse();

        // Transpilation
        std::cout << "CMinus >> Transpiling...\n";

        cminus::Transpiler transpiler;

        std::string cppCode =
            transpiler.transpile(*program);

        // Write generated C++
        writeFile(outputFile, cppCode);

        std::cout
            << "CMinus >> Successfully transpiled!\n"
            << "CMinus >> Output: "
            << outputFile << '\n';

        return 0;

    }
    catch (const std::exception& exception) {

        cminus::ErrorHandler errors;

        errors.fatal(exception.what(), inputFile);

        std::cerr
            << "CMinus >> Compilation failed.\n";

        return 1;
    }

}
