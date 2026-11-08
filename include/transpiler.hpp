
#ifndef CMINUS_TRANSPILER_HPP
#define CMINUS_TRANSPILER_HPP

#include "parser.hpp"

#include <string>

namespace cminus {

    class Transpiler {
    public:
        Transpiler();

        std::string transpile(const Program& program);

    private:
        std::string output;
        int indentation;

        void emitLine(const std::string& line);
        void emitStatement(const Statement& statement);

        std::string emitExpression(const Expression& expression);
        std::string emitLiteral(const LiteralExpr& literal);
        std::string emitIdentifier(const IdentifierExpr& identifier);
        std::string emitBinary(const BinaryExpr& binary);
        std::string emitCall(const CallExpr& call);

        std::string tokenToOperator(TokenType type) const;
    };

}

#endif
