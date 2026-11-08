
#ifndef CMINUS_TRANSPILER_HPP
#define CMINUS_TRANSPILER_HPP

#include "parser.hpp"

#include <memory>
#include <string>

namespace cminus {

    class Transpiler {
    public:
        Transpiler();

        std::string transpile(const Program& program);

    private:
        std::string output;
        int indentation;

        // Output helpers
        void emit(const std::string& text);
        void emitLine(const std::string& text = "");
        std::string getIndentation() const;

        // Declarations
        void transpileInclude(const IncludeDecl& include);
        void transpileFunction(const FunctionDecl& function);

        // Statements
        void transpileStatement(const Statement& statement);
        void transpileRender(const RenderStmt& statement);
        void transpileExpressionStatement(
            const ExpressionStmt& statement
        );

        // Expressions
        std::string transpileExpression(
            const Expression& expression
        );

        std::string transpileLiteral(
            const LiteralExpr& expression
        );

        std::string transpileIdentifier(
            const IdentifierExpr& expression
        );

        std::string transpileBinary(
            const BinaryExpr& expression
        );

        std::string transpileCall(
            const CallExpr& expression
        );

        // Utilities
        std::string tokenToOperator(TokenType type) const;
    };

}

#endif
