
#ifndef CMINUS_PARSER_HPP
#define CMINUS_PARSER_HPP

#include "lexer.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cminus {

    // =========================
    // AST BASE
    // =========================

    struct ASTNode {
        virtual ~ASTNode() = default;
    };

    struct Expression : ASTNode {
        virtual ~Expression() = default;
    };

    struct Statement : ASTNode {
        virtual ~Statement() = default;
    };

    // =========================
    // EXPRESSIONS
    // =========================

    struct LiteralExpr : Expression {
        TokenType type;
        std::string value;

        LiteralExpr(TokenType type, const std::string& value)
            : type(type), value(value) {}
    };

    struct IdentifierExpr : Expression {
        std::string name;

        explicit IdentifierExpr(const std::string& name)
            : name(name) {}
    };

    struct BinaryExpr : Expression {
        std::unique_ptr<Expression> left;
        TokenType operation;
        std::unique_ptr<Expression> right;

        BinaryExpr(
            std::unique_ptr<Expression> left,
            TokenType operation,
            std::unique_ptr<Expression> right
        ) : left(std::move(left)),
            operation(operation),
            right(std::move(right)) {}
    };

    struct CallExpr : Expression {
        std::string name;
        std::vector<std::unique_ptr<Expression>> arguments;

        CallExpr(
            const std::string& name,
            std::vector<std::unique_ptr<Expression>> arguments
        ) : name(name), arguments(std::move(arguments)) {}
    };

    // =========================
    // STATEMENTS
    // =========================

    struct RenderStmt : Statement {
        std::unique_ptr<Expression> value;

        explicit RenderStmt(std::unique_ptr<Expression> value)
            : value(std::move(value)) {}
    };

    struct ExpressionStmt : Statement {
        std::unique_ptr<Expression> expression;

        explicit ExpressionStmt(std::unique_ptr<Expression> expression)
            : expression(std::move(expression)) {}
    };

    // =========================
    // DECLARATIONS
    // =========================

    struct IncludeDecl : ASTNode {
        std::string header;

        explicit IncludeDecl(const std::string& header)
            : header(header) {}
    };

    struct FunctionDecl : ASTNode {
        std::string name;
        std::string returnType;

        std::vector<std::unique_ptr<Statement>> body;

        FunctionDecl(
            const std::string& name,
            const std::string& returnType,
            std::vector<std::unique_ptr<Statement>> body
        ) : name(name),
            returnType(returnType),
            body(std::move(body)) {}
    };

    // =========================
    // PROGRAM
    // =========================

    struct Program : ASTNode {
        std::vector<std::unique_ptr<IncludeDecl>> includes;
        std::vector<std::unique_ptr<FunctionDecl>> functions;
    };

    // =========================
    // PARSER
    // =========================

    class Parser {
    public:
        explicit Parser(const std::vector<Token>& tokens);

        std::unique_ptr<Program> parse();

    private:
        const std::vector<Token>& tokens;
        size_t position;

        const Token& current() const;
        const Token& peek(size_t offset = 1) const;

        Token advance();

        bool check(TokenType type) const;
        bool match(TokenType type);

        Token consume(
            TokenType type,
            const std::string& message
        );

        // Declarations
        std::unique_ptr<IncludeDecl> parseInclude();
        std::unique_ptr<FunctionDecl> parseFunction();

        // Statements
        std::unique_ptr<Statement> parseStatement();
        std::unique_ptr<Statement> parseRender();
        std::unique_ptr<Statement> parseExpressionStatement();

        // Expressions
        std::unique_ptr<Expression> parseExpression();
        std::unique_ptr<Expression> parseEquality();
        std::unique_ptr<Expression> parseComparison();
        std::unique_ptr<Expression> parseTerm();
        std::unique_ptr<Expression> parseFactor();
        std::unique_ptr<Expression> parsePrimary();

        // Helpers
        std::vector<std::unique_ptr<Expression>> parseArguments();

        [[noreturn]] void error(const std::string& message) const;
    };

}

#endif
