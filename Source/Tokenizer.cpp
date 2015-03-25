#define AUTOHEADER_BREAK_ENCAPSULATION_Source_47Tokenizer_46hpp
#include "Tokenizer.hpp"
#include <stdexcept>
EXPORT_INCLUDE("Maybe.hpp")
EXPORT_INCLUDE("iostream")
EXPORT_INCLUDE("Token.hpp")

EXPORT_AS_CLASS(
Tokenizer final { // thang : class not struct
	std::istream &_input; // If you change this, make the class NoCopy.

public:
	explicit Tokenizer(std::istream &input);
};
)

Tokenizer::Tokenizer(std::istream &input)
	: _input(input)
{
}

namespace {
	bool IsIdentifierStartCharacter(int character) {
		return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_';
	}

	bool IsIdentifierCharacter(int character) {
		return IsIdentifierStartCharacter(character) || (character >= '0' && character <= '9');
	}

	bool IsWhitespace(int character) {
		return character == ' ' || character == '\n' || character == '\t' || character == '\r';
	}

	std::string SkipWhitespace(std::istream &input) {
		std::string result;

		int character;
		while (character = input.peek(), character != EOF) {
			if (IsWhitespace(character)) {
				result.push_back(character);
				input.get();
			} else {
				return result;
			}
		}
		return result;
	}

	Token ReadStringLiteral(std::istream &input, TokenType type, int deliminator, std::string precedingWhitespace) {
		std::string value;
		value.push_back(deliminator);
		int character;
		while (character = input.get(), character != EOF) {
			value.push_back(character);
			if (character == '\\') {
				int nextCharacter = input.get();
				if (nextCharacter == EOF)
					throw std::runtime_error("Unexpected end of file.");
				value.push_back(nextCharacter);
			} else if (character == deliminator) {
				break;
			}
		}
		return Token(type, std::move(precedingWhitespace), std::move(value));
	}
}

EXPORT Maybe<Token> ReadToken(Tokenizer &tokenizer) {
	std::string precedingWhitespace;
	for (;;) {
		int character = tokenizer._input.get();
		switch (character) {
			// thang : detect comments
			case EOF:
				return {};
			case ';':
				return Token(TokenType::Semicolon, precedingWhitespace, character);
			case '{':
				return Token(TokenType::LeftBrace, precedingWhitespace, character);
			case '}':
				return Token(TokenType::RightBrace, precedingWhitespace, character);
			case '(':
				return Token(TokenType::LeftParenthesis, precedingWhitespace, character);
			case ')':
				return Token(TokenType::RightParenthesis, precedingWhitespace, character);
			case '\'':
				return ReadStringLiteral(tokenizer._input, TokenType::Unknown, '\'', precedingWhitespace);
			case '"':
				return ReadStringLiteral(tokenizer._input, TokenType::StringLiteral, '"', precedingWhitespace);
			default:
			{
				if (IsIdentifierStartCharacter(character)) {
					std::string result(1, character);
					for (;;) {
						const int character = tokenizer._input.peek();
						if (character == EOF || !IsIdentifierCharacter(character))
							break;
						tokenizer._input.get();
						result.push_back(character);
					}
					return Token(TokenType::Identifier, precedingWhitespace, result);
				} else if (IsWhitespace(character)) {
					precedingWhitespace.push_back(character);
					precedingWhitespace += SkipWhitespace(tokenizer._input);
					break;
				} else {
					return Token(TokenType::Unknown, precedingWhitespace, character);
				}
			}
		}
	}
}

EXPORT Token ReadRequiredToken(Tokenizer &tokenizer) {
	Maybe<Token> maybeToken = ReadToken(tokenizer);
	if (!maybeToken.HasValue())
		throw std::runtime_error("Required token was not found.");
	return maybeToken.GetValue();
}

EXPORT Token ReadRequiredToken(Tokenizer &tokenizer, TokenType type) {
	Token result = ReadRequiredToken(tokenizer);
	if (GetType(result) != type)
		throw std::runtime_error("Required token is of the wrong type.");
	return result;
}
