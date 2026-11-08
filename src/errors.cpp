
#include "errors.hpp"

#include <iostream>

namespace cminus {

    ErrorHandler::ErrorHandler()
        : errorCount(0), warningCount(0) {}

    // =========================
    // LABELS
    // =========================

    std::string ErrorHandler::getLabel(ErrorType type) const {
        switch (type) {
            case ErrorType::INFO:
                return "info";

            case ErrorType::WARNING:
                return "warning";

            case ErrorType::ERROR:
                return "error";

            case ErrorType::FATAL:
                return "fatal error";
        }

        return "unknown";
    }

    // =========================
    // REPORT
    // =========================

    void ErrorHandler::report(
        ErrorType type,
        const std::string& message,
        const std::string& filename,
        int line,
        int column
    ) {
        if (type == ErrorType::ERROR || type == ErrorType::FATAL) {
            errorCount++;
        }

        if (type == ErrorType::WARNING) {
            warningCount++;
        }

        if (!filename.empty()) {
            std::cerr << filename;

            if (line > 0) {
                std::cerr << ":" << line;

                if (column > 0) {
                    std::cerr << ":" << column;
                }
            }

            std::cerr << ": ";
        }

        std::cerr << getLabel(type) << ": "
                  << message << '\n';
    }

    // =========================
    // CONVENIENCE METHODS
    // =========================

    void ErrorHandler::info(
        const std::string& message,
        const std::string& filename,
        int line,
        int column
    ) {
        report(ErrorType::INFO, message, filename, line, column);
    }

    void ErrorHandler::warning(
        const std::string& message,
        const std::string& filename,
        int line,
        int column
    ) {
        report(ErrorType::WARNING, message, filename, line, column);
    }

    void ErrorHandler::error(
        const std::string& message,
        const std::string& filename,
        int line,
        int column
    ) {
        report(ErrorType::ERROR, message, filename, line, column);
    }

    void ErrorHandler::fatal(
        const std::string& message,
        const std::string& filename,
        int line,
        int column
    ) {
        report(ErrorType::FATAL, message, filename, line, column);
    }

    // =========================
    // COUNTERS
    // =========================

    int ErrorHandler::getErrorCount() const {
        return errorCount;
    }

    int ErrorHandler::getWarningCount() const {
        return warningCount;
    }

    bool ErrorHandler::hasErrors() const {
        return errorCount > 0;
    }

    // =========================
    // RESET
    // =========================

    void ErrorHandler::reset() {
        errorCount = 0;
        warningCount = 0;
    }

}
