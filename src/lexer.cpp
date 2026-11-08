
#include "lexer.hpp"

#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace cminus {

    Lexer::Lexer(const std::string& source)
        : source(source), position(0), line(1), column(1) {}

    char Lexer::current() const {
        if (position >= source.size())
            return '\0';

        return source[position];
    }

    char Lexer::peek(size_t offset) const {
        if (position + offset >= source.size())
            return '\0';

        return source[position + offset];
    }

    char Lexer::advance() {
        char ch = current();

        if (ch == '\0')
            return ch;

        position++;

        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }

        return ch;
    }

    void Lexer::skipWhitespace() {
        while (std::isspace(static_cast<unsigned char>(current()))) {
            advance();
        }
    }

    void Lexer::skipComment() {
        // Single-line comment
        if (current() == '/' && peek() == '/') {
            while (current() != '\n' && current() != '\0') {
                advance();
            }
            return;
        }

        // Multiline comment
        if (current() == '/' && peek() == '*') {
            advance();
            advance();

            while (current() != '\0') {
                if (current() == '*' && peek() == '/') {
                    advance();
                    advance();
                    return;
                }

                advance();
            }

            throw std::runtime_error(
                "Unterminated multiline comment at line " +
                std::to_string(line)
            );
        }
    }

    Token Lexer::makeToken(
        TokenType type,
        const std::string& value,
        int tokenLine,
        int tokenColumn
    ) {
        return {type, value, tokenLine, tokenColumn};
    }

    Token Lexer::readIdentifier() {
        int tokenLine = line;
        int tokenColumn = column;

        std::string value;

        while (
            std::isalnum(static_cast<unsigned char>(current())) ||
            current() == '_'
        ) {
            value += advance();
        }

        static const std::unordered_map<std::string, TokenType> keywords = {
            {"include", TokenType::INCLUDE},
            {"func", TokenType::FUNC},
            {"render", TokenType::RENDER}
        };

        auto it = keywords.find(value);

        if (it != keywords.end()) {
            return makeToken(it->second, value, tokenLine, tokenColumn);
        }

        return makeToken(
            TokenType::IDENTIFIER,
            value,
            tokenLine,
            tokenColumn
        );
    }

    Token Lexer::readNumber() {
        int tokenLine = line;
        int tokenColumn = column;

        std::string value;
        bool isFloat = false;

        while (
            std::isdigit(static_cast<unsigned char>(current())) ||
            (current() == '.' &&
             !isFloat &&
             std::isdigit(static_cast<unsigned char>(peek())))
        ) {
            if (current() == '.') {
                isFloat = true;
            }

            value += advance();
        }

        return makeToken(
            isFloat ? TokenType::FLOAT : TokenType::INTEGER,
            value,
            tokenLine,
            tokenColumn
        );
    }

    Token Lexer::readString() {
        int tokenLine = line;
        int tokenColumn = column;

        advance(); // Opening quote

        std::string value;

        while (current() != '"' && current() != '\0') {
            if (current() == '\n') {
                throw std::runtime_error(
                    "Unterminated string at line " +
                    std::to_string(tokenLine)
                );
            }

            if (current() == '\\') {
                advance();

                switch (current()) {
                    case 'n':
                        value += '\n';
                        break;

                    case 't':
                        value += '\t';
                        break;

                    case 'r':
                        value += '\r';
                        break;

                    case '"':
                        value += '"';
                        break;

                    case '\\':
                        value += '\\';
                        break;

                    default:
                        value += '\\';
                        value += current();
                        break;
                }

                if (current() != '\0')
                    advance();

                continue;
            }

            value += advance();
        }

        if (current() == '\0') {
            throw std::runtime_error(
                "Unterminated string at line " +
                std::to_string(tokenLine)
            );
        }

        advance(); // Closing quote

        return makeToken(
            TokenType::STRING,
            value,
            tokenLine,
            tokenColumn
        );
    }

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;

        while (current() != '\0') {
            skipWhitespace();

            if (current() == '\0')
                break;

            if (current() == '/' &&
                (peek() == '/' || peek() == '*')) {
                skipComment();
                continue;
            }

            int tokenLine = line;
            int tokenColumn = column;

            char ch = current();

            if (std::isalpha(static_cast<unsigned char>(ch)) ||
                ch == '_') {
                tokens.push_back(readIdentifier());
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(ch))) {
                tokens.push_back(readNumber());
                continue;
            }

            if (ch == '"') {
                tokens.push_back(readString());
                continue;
            }

            advance();

            switch (ch) {
                case '-':
                    if (current() == '>') {
                        advance();
                        tokens.push_back(makeToken(
                            TokenType::ARROW, "->",
                            tokenLine, tokenColumn
                        ));
                    } else {
                        tokens.push_back(makeToken(
                            TokenType::MINUS, "-",
                            tokenLine, tokenColumn
                        ));
                    }
                    break;

                case '+':
                    tokens.push_back(makeToken(
                        TokenType::PLUS, "+",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '*':
                    tokens.push_back(makeToken(
                        TokenType::MULTIPLY, "*",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '/':
                    tokens.push_back(makeToken(
                        TokenType::DIVIDE, "/",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '=':
                    if (current() == '=') {
                        advance();
                        tokens.push_back(makeToken(
                            TokenType::EQUAL, "==",
                            tokenLine, tokenColumn
                        ));
                    } else {
                        tokens.push_back(makeToken(
                            TokenType::ASSIGN, "=",
                            tokenLine, tokenColumn
                        ));
                    }
                    break;

                case '!':
                    if (current() == '=') {
                        advance();
                        tokens.push_back(makeToken(
                            TokenType::NOT_EQUAL, "!=",
                            tokenLine, tokenColumn
                        ));
                    } else {
                        tokens.push_back(makeToken(
                            TokenType::UNKNOWN, "!",
                            tokenLine, tokenColumn
                        ));
                    }
                    break;

                case '<':
                    if (current() == '=') {
                        advance();
                        tokens.push_back(makeToken(
                            TokenType::LESS_EQUAL, "<=",
                            tokenLine, tokenColumn
                        ));
                    } else {
                        tokens.push_back(makeToken(
                            TokenType::LESS, "<",
                            tokenLine, tokenColumn
                        ));
                    }
                    break;

                case '>':
                    if (current() == '=') {
                        advance();
                        tokens.push_back(makeToken(
                            TokenType::GREATER_EQUAL, ">=",
                            tokenLine, tokenColumn
                        ));
                    } else {
                        tokens.push_back(makeToken(
                            TokenType::GREATER, ">",
                            tokenLine, tokenColumn
                        ));
                    }
                    break;

                case '(':
                    tokens.push_back(makeToken(
                        TokenType::LEFT_PAREN, "(",
                        tokenLine, tokenColumn
                    ));
                    break;

                case ')':
                    tokens.push_back(makeToken(
                        TokenType::RIGHT_PAREN, ")",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '{':
                    tokens.push_back(makeToken(
                        TokenType::LEFT_BRACE, "{",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '}':
                    tokens.push_back(makeToken(
                        TokenType::RIGHT_BRACE, "}",
                        tokenLine, tokenColumn
                    ));
                    break;

                case ';':
                    tokens.push_back(makeToken(
                        TokenType::SEMICOLON, ";",
                        tokenLine, tokenColumn
                    ));
                    break;

                case ',':
                    tokens.push_back(makeToken(
                        TokenType::COMMA, ",",
                        tokenLine, tokenColumn
                    ));
                    break;

                case '.':
                    tokens.push_back(makeToken(
                        TokenType::DOT, ".",
                        tokenLine, tokenColumn
                    ));
                    break;

                default:
                    tokens.push_back(makeToken(
                        TokenType::UNKNOWN,
                        std::string(1, ch),
                        tokenLine,
                        tokenColumn
                    ));
                    break;
            }
        }

        tokens.push_back(makeToken(
            TokenType::END_OF_FILE,
            "",
            line,
            column
        ));

        return tokens;
    }

}
