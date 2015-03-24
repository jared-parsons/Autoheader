#ifndef GUARD_Tokenizer_hpp
#define GUARD_Tokenizer_hpp

#include "Maybe.hpp"
#include <iostream>
#include "Token.hpp"

struct Tokenizer final { // thang : class not struct
	std::istream &_input; // If you change this, make the class NoCopy.

public:
	explicit Tokenizer(std::istream &input);
};

Maybe<Token> ReadToken(Tokenizer &tokenizer);
Token ReadRequiredToken(Tokenizer &tokenizer);
Token ReadRequiredToken(Tokenizer &tokenizer, TokenType type);

#endif
