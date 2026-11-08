
#ifndef CMINUS_ERRORS_HPP
#define CMINUS_ERRORS_HPP

#include <string>

namespace cminus {

    enum class ErrorType {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    struct Diagnostic {
        ErrorType type;
        std::string message;
        std::string filename;
        int line;
        int column;
    };

    class ErrorHandler {
    public:
        ErrorHandler();

        void report(
            ErrorType type,
            const std::string& message,
            const std::string& filename = "",
            int line = 0,
            int column = 0
        );

        void info(
            const std::string& message,
            const std::string& filename = "",
            int line = 0,
            int column = 0
        );

        void warning(
            const std::string& message,
            const std::string& filename = "",
            int line = 0,
            int column = 0
        );

        void error(
            const std::string& message,
            const std::string& filename = "",
            int line = 0,
            int column = 0
        );

        void fatal(
            const std::string& message,
            const std::string& filename = "",
            int line = 0,
            int column = 0
        );

        int getErrorCount() const;
        int getWarningCount() const;

        bool hasErrors() const;

        void reset();

    private:
        int errorCount;
        int warningCount;

        std::string getLabel(ErrorType type) const;
    };

}

#endif
