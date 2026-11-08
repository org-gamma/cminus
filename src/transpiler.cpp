
#include "transpiler.hpp"

#include <stdexcept>

namespace cminus {

    Transpiler::Transpiler()
        : output(""), indentation(0) {
    }

    std::string Transpiler::transpile(const Program& program) {
        output.clear();
        indentation = 0;

        // Generate includes.
        for (const auto& include : program.includes) {
            emitLine("#include <" + include->header + ">");
        }

        if (!program.includes.empty() && !program.functions.empty()) {
            emitLine("");
        }

        // Generate functions.
        for (size_t i = 0; i < program.functions.size(); ++i) {
            const auto& function = program.functions[i];

            emitLine(function->returnType + " " + function->name + "() {");

            indentation++;

            for (const auto& statement : function->body) {
                emitStatement(*statement);
            }

            indentation--;

            emitLine("}");

            if (i + 1 < program.functions.size()) {
                emitLine("");
            }
        }

        return output;
    }

    void Transpiler::emitLine(const std::string& line) {
        for (int i = 0; i < indentation; ++i) {
            output += "    ";
        }

        output += line;
        output += '\n';
    }

    void Transpiler::emitStatement(const Statement& statement) {
        if (const auto* render =
                dynamic_cast<const RenderStmt*>(&statement)) {

            emitLine("return " + emitExpression(*render->value) + ";");
            return;
        }

        if (const auto* expression =
                dynamic_cast<const ExpressionStmt*>(&statement)) {

            emitLine(emitExpression(*expression->expression) + ";");
            return;
        }

        throw std::runtime_error("Unknown statement type.");
    }

    std::string Transpiler::emitExpression(
        const Expression& expression
    ) {
        if (const auto* literal =
                dynamic_cast<const LiteralExpr*>(&expression)) {
            return emitLiteral(*literal);
        }

        if (const auto* identifier =
                dynamic_cast<const IdentifierExpr*>(&expression)) {
            return emitIdentifier(*identifier);
        }

        if (const auto* binary =
                dynamic_cast<const BinaryExpr*>(&expression)) {
            return emitBinary(*binary);
        }

        if (const auto* call =
                dynamic_cast<const CallExpr*>(&expression)) {
            return emitCall(*call);
        }

        throw std::runtime_error("Unknown expression type.");
    }

    std::string Transpiler::emitLiteral(const LiteralExpr& literal) {
        if (literal.type == TokenType::STRING) {
            std::string result = "\"";

            for (char character : literal.value) {
                switch (character) {
                    case '\n':
                        result += "\\n";
                        break;

                    case '\t':
                        result += "\\t";
                        break;

                    case '\r':
                        result += "\\r";
                        break;

                    case '\\':
                        result += "\\\\";
                        break;

                    case '"':
                        result += "\\\"";
                        break;

                    default:
                        result += character;
                        break;
                }
            }

            result += "\"";
            return result;
        }

        return literal.value;
    }

    std::string Transpiler::emitIdentifier(
        const IdentifierExpr& identifier
    ) {
        return identifier.name;
    }

    std::string Transpiler::emitBinary(const BinaryExpr& binary) {
        return "(" +
            emitExpression(*binary.left) +
            " " + tokenToOperator(binary.operation) + " " +
            emitExpression(*binary.right) +
            ")";
    }

    std::string Transpiler::emitCall(const CallExpr& call) {
        std::string result;

        // CMinus standard library functions.
        if (call.name == "print" ||
            call.name == "println" ||
            call.name == "printf") {
            result += "cminus::";
        }

        result += call.name;
        result += "(";

        for (size_t i = 0; i < call.arguments.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }

            result += emitExpression(*call.arguments[i]);
        }

        result += ")";

        return result;
    }

    std::string Transpiler::tokenToOperator(TokenType type) const {
        switch (type) {
            case TokenType::PLUS:
                return "+";

            case TokenType::MINUS:
                return "-";

            case TokenType::MULTIPLY:
                return "*";

            case TokenType::DIVIDE:
                return "/";

            case TokenType::EQUAL:
                return "==";

            case TokenType::NOT_EQUAL:
                return "!=";

            case TokenType::LESS:
                return "<";

            case TokenType::GREATER:
                return ">";

            case TokenType::LESS_EQUAL:
                return "<=";

            case TokenType::GREATER_EQUAL:
                return ">=";

            default:
                throw std::runtime_error("Unknown binary operator.");
        }
    }

}
