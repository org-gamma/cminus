
#include "transpiler.hpp"

#include <stdexcept>
#include <utility>

namespace cminus {

    Transpiler::Transpiler()
        : indentation(0) {}

    // =========================
    // OUTPUT HELPERS
    // =========================

    void Transpiler::emit(const std::string& text) {
        output += text;
    }

    void Transpiler::emitLine(const std::string& text) {
        output += getIndentation();
        output += text;
        output += '\n';
    }

    std::string Transpiler::getIndentation() const {
        return std::string(indentation * 4, ' ');
    }

    // =========================
    // MAIN TRANSPILER
    // =========================

    std::string Transpiler::transpile(const Program& program) {
        output.clear();
        indentation = 0;

        // Generate includes
        for (const auto& include : program.includes) {
            transpileInclude(*include);
        }

        if (!program.includes.empty())
            emitLine();

        // Generate functions
        for (size_t i = 0; i < program.functions.size(); ++i) {
            transpileFunction(*program.functions[i]);

            if (i + 1 < program.functions.size())
                emitLine();
        }

        return output;
    }

    // =========================
    // INCLUDE DECLARATIONS
    // =========================

    void Transpiler::transpileInclude(const IncludeDecl& include) {
        emitLine("#include <" + include.header + ">");
    }

    // =========================
    // FUNCTION DECLARATIONS
    // =========================

    void Transpiler::transpileFunction(
        const FunctionDecl& function
    ) {
        emitLine(
            function.returnType + " " +
            function.name + "() {"
        );

        indentation++;

        for (const auto& statement : function.body) {
            transpileStatement(*statement);
        }

        indentation--;

        emitLine("}");
    }

    // =========================
    // STATEMENTS
    // =========================

    void Transpiler::transpileStatement(
        const Statement& statement
    ) {
        if (auto render = dynamic_cast<const RenderStmt*>(&statement)) {
            transpileRender(*render);
            return;
        }

        if (auto expression =
            dynamic_cast<const ExpressionStmt*>(&statement)) {
            transpileExpressionStatement(*expression);
            return;
        }

        throw std::runtime_error(
            "Transpiler Error: Unknown statement type."
        );
    }

    void Transpiler::transpileRender(
        const RenderStmt& statement
    ) {
        emitLine(
            "return " +
            transpileExpression(*statement.value) +
            ";"
        );
    }

    void Transpiler::transpileExpressionStatement(
        const ExpressionStmt& statement
    ) {
        emitLine(
            transpileExpression(*statement.expression) + ";"
        );
    }

    // =========================
    // EXPRESSIONS
    // =========================

    std::string Transpiler::transpileExpression(
        const Expression& expression
    ) {
        if (auto literal = dynamic_cast<const LiteralExpr*>(&expression)) {
            return transpileLiteral(*literal);
        }

        if (auto identifier =
            dynamic_cast<const IdentifierExpr*>(&expression)) {
            return transpileIdentifier(*identifier);
        }

        if (auto binary = dynamic_cast<const BinaryExpr*>(&expression)) {
            return transpileBinary(*binary);
        }

        if (auto call = dynamic_cast<const CallExpr*>(&expression)) {
            return transpileCall(*call);
        }

        throw std::runtime_error(
            "Transpiler Error: Unknown expression type."
        );
    }

    // =========================
    // LITERALS
    // =========================

    std::string Transpiler::transpileLiteral(
        const LiteralExpr& expression
    ) {
        if (expression.type == TokenType::STRING) {
            std::string result = "\"";

            for (char ch : expression.value) {
                switch (ch) {
                    case '\\':
                        result += "\\\\";
                        break;

                    case '"':
                        result += "\\\"";
                        break;

                    case '\n':
                        result += "\\n";
                        break;

                    case '\t':
                        result += "\\t";
                        break;

                    case '\r':
                        result += "\\r";
                        break;

                    default:
                        result += ch;
                        break;
                }
            }

            result += "\"";

            return result;
        }

        return expression.value;
    }

    // =========================
    // IDENTIFIERS
    // =========================

    std::string Transpiler::transpileIdentifier(
        const IdentifierExpr& expression
    ) {
        return expression.name;
    }

    // =========================
    // BINARY EXPRESSIONS
    // =========================

    std::string Transpiler::transpileBinary(
        const BinaryExpr& expression
    ) {
        return "(" +
            transpileExpression(*expression.left) +
            " " +
            tokenToOperator(expression.operation) +
            " " +
            transpileExpression(*expression.right) +
            ")";
    }

    // =========================
    // FUNCTION CALLS
    // =========================

    std::string Transpiler::transpileCall(
        const CallExpr& expression
    ) {
        std::string result = expression.name + "(";

        for (size_t i = 0; i < expression.arguments.size(); ++i) {
            result += transpileExpression(
                *expression.arguments[i]
            );

            if (i + 1 < expression.arguments.size())
                result += ", ";
        }

        result += ")";

        return result;
    }

    // =========================
    // OPERATORS
    // =========================

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

            case TokenType::ASSIGN:
                return "=";

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
                throw std::runtime_error(
                    "Transpiler Error: Unknown operator."
                );
        }
    }

}
