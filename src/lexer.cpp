#include "kal/lexer.hpp"

#include <cctype>
#include <iostream>
#include <string>

std::string line;
int linePos = 0;

std::string identStr;
double numVal;

bool loadNextLine() {
  if (!std::getline(std::cin, line)) return false;
  line += '\n';
  linePos = 0;
  return true;
}

int nextChar() {
  if (linePos >= (int)line.size()) {
    if (!loadNextLine()) return EOF;
  }
  return line[linePos++];
}

int getToken() {
  static int c = ' ';

  while (std::isspace(c)) c = nextChar();

  if (std::isalpha(c)) {
    identStr = c;
    while (std::isalpha(c = nextChar())) identStr += c;

    if (identStr == "fn") return token_fn;
    if (identStr == "extern") return token_extern;

    return token_ident;
  }

  if (std::isdigit(c) or c == '.') {
    std::string numStr;

    do {
      numStr += c;
      c = nextChar();
    } while (std::isdigit(c) or c == '.');

    numVal = std::stod(numStr.c_str());

    return token_num;
  }

  if (c == '#') {
    do c = nextChar();
    while (c != '\n' and c != EOF and c != '\r');

    if (c != EOF) return getToken();
  }

  if (c == EOF) return token_eof;

  int x = c;
  c = nextChar();

  return x;
}
