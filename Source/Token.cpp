#define AUTOHEADER_BREAK_ENCAPSULATION_Source_47Token_46hpp
#include "Token.hpp"
EXPORT_INCLUDE("string")

HEADER(
enum class TokenType {
	Unknown,
	Identifier,
	Semicolon,
	LeftBrace,
	RightBrace,
	LeftParenthesis,
	RightParenthesis,
	StringLiteral,
};
)

EXPORT_AS_CLASS(
Token final {
	TokenType _type;
	std::string _precedingWhitespace;
	std::string _value;

public:
	explicit Token(TokenType type, std::string precedingWhitespace, std::string value);
	explicit Token(TokenType type, std::string precedingWhitespace, int character);
};
);

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

EXPORT TokenType GetType(const Token &token) {
	return token._type;
}

EXPORT const std::string &GetValue(const Token &token) {
	return token._value;
}

EXPORT std::ostream &operator<<(std::ostream &output, const Token &token) {
	output << token._precedingWhitespace << token._value;
	return output;
}
