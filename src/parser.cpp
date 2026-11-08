
#include "parser.hpp"

#include <stdexcept>
#include <utility>

namespace cminus {

    // =========================
    // CONSTRUCTOR
    // =========================

    Parser::Parser(const std::vector<Token>& tokens)
        : tokens(tokens), position(0) {}

    // =========================
    // TOKEN ACCESS
    // =========================

    const Token& Parser::current() const {
        return tokens[position];
    }

    const Token& Parser::peek(size_t offset) const {
        size_t index = position + offset;

        if (index >= tokens.size())
            return tokens.back();

        return tokens[index];
    }

    Token Parser::advance() {
        Token token = current();

        if (!check(TokenType::END_OF_FILE))
            position++;

        return token;
    }

    bool Parser::check(TokenType type) const {
        return current().type == type;
    }

    bool Parser::match(TokenType type) {
        if (!check(type))
            return false;

        advance();
        return true;
    }

    Token Parser::consume(
        TokenType type,
        const std::string& message
    ) {
        if (check(type))
            return advance();

        error(message);
    }

    [[noreturn]] void Parser::error(
        const std::string& message
    ) const {
        throw std::runtime_error(
            "Parser Error at line " +
            std::to_string(current().line) +
            ", column " +
            std::to_string(current().column) +
            ": " + message
        );
    }

    // =========================
    // PROGRAM PARSING
    // =========================

    std::unique_ptr<Program> Parser::parse() {
        auto program = std::make_unique<Program>();

        while (!check(TokenType::END_OF_FILE)) {

            if (check(TokenType::INCLUDE)) {
                program->includes.push_back(parseInclude());
            }
            else if (check(TokenType::FUNC)) {
                program->functions.push_back(parseFunction());
            }
            else {
                error("Expected 'include' or 'func'.");
            }
        }

        return program;
    }

    // =========================
    // INCLUDE DECLARATIONS
    // =========================

    std::unique_ptr<IncludeDecl> Parser::parseInclude() {
        consume(
            TokenType::INCLUDE,
            "Expected 'include'."
        );

        consume(
            TokenType::LESS,
            "Expected '<' after include."
        );

        std::string header;

        while (
            !check(TokenType::GREATER) &&
            !check(TokenType::END_OF_FILE)
        ) {
            header += advance().value;
        }

        consume(
            TokenType::GREATER,
            "Expected '>' after header name."
        );

        consume(
            TokenType::SEMICOLON,
            "Expected ';' after include declaration."
        );

        if (header.empty())
            error("Header name cannot be empty.");

        return std::make_unique<IncludeDecl>(header);
    }

    // =========================
    // FUNCTION DECLARATIONS
    // =========================

    std::unique_ptr<FunctionDecl> Parser::parseFunction() {
        consume(
            TokenType::FUNC,
            "Expected 'func'."
        );

        Token name = consume(
            TokenType::IDENTIFIER,
            "Expected function name."
        );

        consume(
            TokenType::LEFT_PAREN,
            "Expected '(' after function name."
        );

        consume(
            TokenType::RIGHT_PAREN,
            "Expected ')' after function parameters."
        );

        consume(
            TokenType::ARROW,
            "Expected '->' before return type."
        );

        Token returnType = consume(
            TokenType::IDENTIFIER,
            "Expected return type."
        );

        consume(
            TokenType::LEFT_BRACE,
            "Expected '{' before function body."
        );

        std::vector<std::unique_ptr<Statement>> body;

        while (
            !check(TokenType::RIGHT_BRACE) &&
            !check(TokenType::END_OF_FILE)
        ) {
            body.push_back(parseStatement());
        }

        consume(
            TokenType::RIGHT_BRACE,
            "Expected '}' after function body."
        );

        return std::make_unique<FunctionDecl>(
            name.value,
            returnType.value,
            std::move(body)
        );
    }

    // =========================
    // STATEMENTS
    // =========================

    std::unique_ptr<Statement> Parser::parseStatement() {
        if (check(TokenType::RENDER))
            return parseRender();

        return parseExpressionStatement();
    }

    std::unique_ptr<Statement> Parser::parseRender() {
        consume(
            TokenType::RENDER,
            "Expected 'render'."
        );

        auto value = parseExpression();

        consume(
            TokenType::SEMICOLON,
            "Expected ';' after render statement."
        );

        return std::make_unique<RenderStmt>(
            std::move(value)
        );
    }

    std::unique_ptr<Statement>
    Parser::parseExpressionStatement() {

        auto expression = parseExpression();

        consume(
            TokenType::SEMICOLON,
            "Expected ';' after expression."
        );

        return std::make_unique<ExpressionStmt>(
            std::move(expression)
        );
    }

    // =========================
    // EXPRESSIONS
    // =========================

    std::unique_ptr<Expression> Parser::parseExpression() {
        return parseEquality();
    }

    // Equality: ==, !=

    std::unique_ptr<Expression> Parser::parseEquality() {
        auto expression = parseComparison();

        while (
            check(TokenType::EQUAL) ||
            check(TokenType::NOT_EQUAL)
        ) {
            TokenType operation = advance().type;

            auto right = parseComparison();

            expression = std::make_unique<BinaryExpr>(
                std::move(expression),
                operation,
                std::move(right)
            );
        }

        return expression;
    }

    // Comparison: <, >, <=, >=

    std::unique_ptr<Expression> Parser::parseComparison() {
        auto expression = parseTerm();

        while (
            check(TokenType::LESS) ||
            check(TokenType::GREATER) ||
            check(TokenType::LESS_EQUAL) ||
            check(TokenType::GREATER_EQUAL)
        ) {
            TokenType operation = advance().type;

            auto right = parseTerm();

            expression = std::make_unique<BinaryExpr>(
                std::move(expression),
                operation,
                std::move(right)
            );
        }

        return expression;
    }

    // Addition and subtraction

    std::unique_ptr<Expression> Parser::parseTerm() {
        auto expression = parseFactor();

        while (
            check(TokenType::PLUS) ||
            check(TokenType::MINUS)
        ) {
            TokenType operation = advance().type;

            auto right = parseFactor();

            expression = std::make_unique<BinaryExpr>(
                std::move(expression),
                operation,
                std::move(right)
            );
        }

        return expression;
    }

    // Multiplication and division

    std::unique_ptr<Expression> Parser::parseFactor() {
        auto expression = parsePrimary();

        while (
            check(TokenType::MULTIPLY) ||
            check(TokenType::DIVIDE)
        ) {
            TokenType operation = advance().type;

            auto right = parsePrimary();

            expression = std::make_unique<BinaryExpr>(
                std::move(expression),
                operation,
                std::move(right)
            );
        }

        return expression;
    }

    // =========================
    // PRIMARY EXPRESSIONS
    // =========================

    std::unique_ptr<Expression> Parser::parsePrimary() {

        if (
            check(TokenType::INTEGER) ||
            check(TokenType::FLOAT) ||
            check(TokenType::STRING)
        ) {
            Token token = advance();

            return std::make_unique<LiteralExpr>(
                token.type,
                token.value
            );
        }

        if (check(TokenType::IDENTIFIER)) {
            std::string name = advance().value;

            if (match(TokenType::LEFT_PAREN)) {
                auto arguments = parseArguments();

                consume(
                    TokenType::RIGHT_PAREN,
                    "Expected ')' after function arguments."
                );

                return std::make_unique<CallExpr>(
                    name,
                    std::move(arguments)
                );
            }

            return std::make_unique<IdentifierExpr>(name);
        }

        if (match(TokenType::LEFT_PAREN)) {
            auto expression = parseExpression();

            consume(
                TokenType::RIGHT_PAREN,
                "Expected ')' after expression."
            );

            return expression;
        }

        error("Expected an expression.");
    }

    // =========================
    // FUNCTION ARGUMENTS
    // =========================

    std::vector<std::unique_ptr<Expression>>
    Parser::parseArguments() {

        std::vector<std::unique_ptr<Expression>> arguments;

        if (check(TokenType::RIGHT_PAREN))
            return arguments;

        do {
            arguments.push_back(parseExpression());
        } while (match(TokenType::COMMA));

        return arguments;
    }

}
