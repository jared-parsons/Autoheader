#ifndef AUTOHEADER_GUARD_Source_47Token_46hpp
#define AUTOHEADER_GUARD_Source_47Token_46hpp
#include "AutoHeader.hpp"
#include "string"
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
#ifdef AUTOHEADER_BREAK_ENCAPSULATION_Source_47Token_46hpp
struct
#else
class
#endif
Token final {
	TokenType _type;
	std::string _precedingWhitespace;
	std::string _value;

public:
	explicit Token(TokenType type, std::string precedingWhitespace, std::string value);
	explicit Token(TokenType type, std::string precedingWhitespace, int character);
};
TokenType GetType(const Token &token);
const std::string &GetValue(const Token &token);
std::ostream &operator<<(std::ostream &output, const Token &token);
#endif
