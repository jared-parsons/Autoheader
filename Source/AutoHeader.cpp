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

	void ParseExportAsClassBlock(Tokenizer &tokenizer, std::ostream &output, const Maybe<std::string> &encapsulationBreaker) {
		if (!encapsulationBreaker.HasValue())
			throw std::runtime_error("EXPORT_AS_CLASS may not be used unless an output filename has been set (-o).");

		output << "#ifdef " << encapsulationBreaker.GetValue() << "\n";
		output << "struct\n";
		output << "#else\n";
		output << "class\n";
		output << "#endif\n";
		ParseHeaderBlock(tokenizer, output);
	}

	std::string EscapeString(std::string string) {
		std::string result;
		for (char character : string) {
			if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9')) {
				result.push_back(character);
			} else {
				result += "_" + std::to_string(static_cast<std::uint32_t>(character));
			}
		}
		return result;
	}

	std::string GenerateGuardName(std::string outputFilename) {
		return "AUTOHEADER_GUARD_" + EscapeString(outputFilename);
	}

	void PrintOpeningHeaderGuard(std::ostream &output, const std::string &guard) {
		output << "#ifndef " << guard << "\n";
		output << "#define " << guard << "\n";
	}

	void PrintClosingHeaderGuard(std::ostream &output) {
		output << "#endif\n";
	}

	void ParseFile(std::istream &input, std::ostream &output, const std::string &guard, const Maybe<std::string> &encapsulationBreaker) {
		PrintOpeningHeaderGuard(output, guard);
		output << "#include \"AutoHeader.hpp\"\n";
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
				} else if (GetValue(token) == "EXPORT_AS_CLASS") {
					ParseExportAsClassBlock(tokenizer, output, encapsulationBreaker);
				}
			}
		}

		PrintClosingHeaderGuard(output);
	}
}

int main(int argc, char * const argv[]) {
	try {
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

		// Setup guard.
		if (!guardSet) {
			if (outputFilenameSet) {
				guard = GenerateGuardName(outputFilename);
			} else {
				std::cerr << "If no output file is specified (-o), then a guard name must be set (-g).\n";
				return 1;
			}
		}

		// Setup output file.
		std::ofstream outputFile;
		if (outputFilenameSet) {
			outputFile.open(outputFilename); // thang : error check?
		}
		std::ostream &output = outputFilenameSet ? outputFile : std::cout;

		// Setup encapsulation breaker.
		Maybe<std::string> encapsulationBreaker;
		if (outputFilenameSet) {
			encapsulationBreaker = "AUTOHEADER_BREAK_ENCAPSULATION_" + EscapeString(outputFilename);
		}

		// And go..
		ParseFile(std::cin, output, guard, encapsulationBreaker);
	} catch (std::exception &exception) {
		std::cerr << "ERROR: " << exception.what() << "\n";
		return 1;
	}
	return 0;
}
