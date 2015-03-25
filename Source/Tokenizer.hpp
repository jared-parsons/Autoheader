#ifndef AUTOHEADER_GUARD_Source_47Tokenizer_46hpp
#define AUTOHEADER_GUARD_Source_47Tokenizer_46hpp
#include "AutoHeader.hpp"
#include "Maybe.hpp"
#include "iostream"
#include "Token.hpp"
#ifdef AUTOHEADER_BREAK_ENCAPSULATION_Source_47Tokenizer_46hpp
struct
#else
class
#endif
Tokenizer final { // thang : class not struct
	std::istream &_input; // If you change this, make the class NoCopy.

public:
	explicit Tokenizer(std::istream &input);
};
Maybe<Token> ReadToken(Tokenizer &tokenizer);
Token ReadRequiredToken(Tokenizer &tokenizer);
Token ReadRequiredToken(Tokenizer &tokenizer, TokenType type);
#endif
