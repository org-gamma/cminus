
#ifndef CMINUS_LEXER_HPP
#define CMINUS_LEXER_HPP

#include <string>
#include <vector>

namespace cminus {

    enum class TokenType {
        // Keywords
        INCLUDE,
        FUNC,
        RENDER,

        // Identifiers and literals
        IDENTIFIER,
        INTEGER,
        FLOAT,
        STRING,

        // Operators
        ARROW,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        ASSIGN,
        EQUAL,
        NOT_EQUAL,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,

        // Punctuation
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        SEMICOLON,
        COMMA,
        DOT,

        // Special
        END_OF_FILE,
        UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string value;
        int line;
        int column;
    };

    class Lexer {
    public:
        explicit Lexer(const std::string& source);

        std::vector<Token> tokenize();

    private:
        std::string source;

        size_t position;
        int line;
        int column;

        char current() const;
        char peek(size_t offset = 1) const;

        char advance();
        void skipWhitespace();
        void skipComment();

        Token makeToken(
            TokenType type,
            const std::string& value,
            int tokenLine,
            int tokenColumn
        );

        Token readIdentifier();
        Token readNumber();
        Token readString();
    };

}

#endif
