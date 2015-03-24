#include "Tokenizer.hpp"
#include <stdexcept>

namespace {
	void ParseExportInclude(Tokenizer &tokenizer, std::ostream &output) {
		ReadRequiredToken(tokenizer, TokenType::LeftParenthesis);
		const Token token = ReadRequiredToken(tokenizer, TokenType::StringLiteral);
		ReadRequiredToken(tokenizer, TokenType::RightParenthesis);

		output << "#include " << GetValue(token) << "\n"; // thang : put a newline before this?
	}

	void ParseHeaderBlock(Tokenizer &tokenizer, std::ostream &output) {
		ReadRequiredToken(tokenizer, TokenType::LeftParenthesis);
		std::size_t parenthesisCount = 1;
		bool first = true;
		for (;;) {
			Maybe<Token> maybeToken = ReadToken(tokenizer);
			if (!maybeToken.HasValue())
				throw std::runtime_error("Unexpected end of file.");
			const Token token = std::move(maybeToken.GetValue());
			switch (GetType(token)) {
				case TokenType::LeftParenthesis:
					++parenthesisCount;
					break;
				case TokenType::RightParenthesis:
					--parenthesisCount;
					if (parenthesisCount == 0) {
						output << "\n";
						return;
					}
					break;
				default:
					break;
			}
			if (first) {
				output << GetValue(token);
				first = false;
			} else {
				output << token;
			}
		}
	}

	void ParseExportBlock(Tokenizer &tokenizer, std::ostream &output) {
		// thang : what about default parameters?
		// thang : this assumes braces cannot appear before the body.

		// Skip to first brace.
		bool first = true;
		for (;;) {
			Maybe<Token> maybeToken = ReadToken(tokenizer);
			if (!maybeToken.HasValue())
				throw std::runtime_error("Unexpected end of file.");
			const Token token = std::move(maybeToken.GetValue());
			if (GetType(token) == TokenType::LeftBrace) {
				output << ";\n";
				return;
			}
			if (first) {
				output << GetValue(token);
				first = false;
			} else {
				output << token;
			}
		}
	}

	void ParseFile(std::istream &input, std::ostream &output) {
		Tokenizer tokenizer(input);

		for (;;) {
			Maybe<Token> maybeToken = ReadToken(tokenizer);
			if (!maybeToken.HasValue())
				return;
			const Token token = std::move(maybeToken.GetValue());
			if (GetType(token) == TokenType::Identifier) {
				if (GetValue(token) == "EXPORT_INCLUDE") {
					ParseExportInclude(tokenizer, output);
				} else if (GetValue(token) == "HEADER") {
					ParseHeaderBlock(tokenizer, output);
				} else if (GetValue(token) == "EXPORT") {
					ParseExportBlock(tokenizer, output);
				}
			}
		}
	}
}

int main() {
	ParseFile(std::cin, std::cout);
	return 0;
}
