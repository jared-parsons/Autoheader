#ifndef GUARD_Token_hpp
#define GUARD_Token_hpp

#include <string>

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

struct Token final { // thang : class not struct
	TokenType _type;
	std::string _precedingWhitespace;
	std::string _value;

public:
	explicit Token(TokenType type, std::string precedingWhitespace, std::string value);
	explicit Token(TokenType type, std::string precedingWhitespace, int character);

/*	std::string GetPrecedingWhitespace() const {
		return _precedingWhitespace;
	}

	void Print(std::ostream &output) const {
		output << _precedingWhitespace << _value;
	}*/
};

inline TokenType GetType(const Token &token) {
	return token._type;
}

inline const std::string &GetValue(const Token &token) {
	return token._value;
}

#endif
