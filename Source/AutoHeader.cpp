#include "Tokenizer.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fstream>

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

	std::string GenerateGuardName(std::string outputFilename) {
		std::string result = "GUARD_";
		for (char character : outputFilename) {
			if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9')) {
				result.push_back(character);
			} else {
				result += "_" + std::to_string(static_cast<std::uint32_t>(character));
			}
		}
		return result;
	}

	void PrintOpeningHeaderGuard(std::ostream &output, const std::string &guard) {
		output << "#ifndef " << guard << "\n";
		output << "#define " << guard << "\n";
	}

	void PrintClosingHeaderGuard(std::ostream &output) {
		output << "#endif\n";
	}

	void ParseFile(std::istream &input, std::ostream &output, const std::string &guard) {
		PrintOpeningHeaderGuard(output, guard);
		Tokenizer tokenizer(input);

		for (;;) {
			Maybe<Token> maybeToken = ReadToken(tokenizer);
			if (!maybeToken.HasValue())
				break;
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

		PrintClosingHeaderGuard(output);
	}
}

int main(int argc, char * const argv[]) {
	std::string guard;
	bool guardSet = false;
	std::string outputFilename;
	bool outputFilenameSet = false;

	int option;
	while ((option = getopt(argc, argv, "o:g:")) != -1) {
		switch (option) {
			case 'o':
				// thang : check outputFileSet?
				outputFilename = optarg;
				outputFilenameSet = true;
				break;
			case 'g':
				// thang : check guardSet?
				guard = optarg;
				guardSet = true;
				break;
		}
	}

	if (!guardSet) {
		if (outputFilenameSet) {
			guard = GenerateGuardName(outputFilename);
		} else {
			std::cerr << "If no output file is specified (-o), then a guard name must be set (-g).\n";
			return 1;
		}
	}

	std::ofstream outputFile;
	if (outputFilenameSet) {
		outputFile.open(outputFilename); // thang : error check?
	}
	std::ostream &output = outputFilenameSet ? outputFile : std::cout;
	ParseFile(std::cin, output, guard);
	return 0;
}
