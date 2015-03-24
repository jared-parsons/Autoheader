#include "Tokenizer.hpp"

namespace {
	void ParseExportInclude(Tokenizer &tokenizer, std::ostream &output) {
		ReadRequiredToken(tokenizer, TokenType::LeftParenthesis);
		const Token token = ReadRequiredToken(tokenizer, TokenType::StringLiteral);
		ReadRequiredToken(tokenizer, TokenType::RightParenthesis);

		output << "#include " << GetValue(token) << "\n"; // thang : put a newline before this?
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
				}
			}
		}
	}
}

int main() {
	ParseFile(std::cin, std::cout);
	return 0;
}
