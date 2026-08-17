#ifndef smv_kal_lexer_hpp
#define smv_kal_lexer_hpp

#include <string>

enum Token {
  token_eof = -1,
  token_fn = -2,
  token_extern = -3,
  token_ident = -4,
  token_num = -5,
};

extern std::string identStr;
extern double numVal;

bool loadNextLine();
int getToken();

#endif
