#include "Token.hpp"

Token::Token(TokenType type, std::string precedingWhitespace, std::string value) {
	_type = type;
	_precedingWhitespace = std::move(precedingWhitespace);
	_value = std::move(value);
}

Token::Token(TokenType type, std::string precedingWhitespace, int character) {
	_type = type;
	_precedingWhitespace = std::move(precedingWhitespace);
	_value = std::string(1, character);
}
