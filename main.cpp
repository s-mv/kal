#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <ostream>

#include "kal/codegen.hpp"
#include "kal/lexer.hpp"
#include "kal/parser.hpp"

void handleDef() {
  if (auto ast = parseFnDef()) {
    if (auto* ir = ast->codegen()) {
      std::cerr << "function def:\n";
      ir->print(llvm::errs());
      std::cerr << "\n";
    }
  } else
    skipToNextStatement();
}

void handleExtern() {
  if (auto ast = parseExtern()) {
    if (auto* ir = ast->codegen()) {
      std::cerr << "extern ir:\n";
      ir->print(llvm::errs());
      std::cerr << "\n";
    }
  } else
    skipToNextStatement();
}

void handleTopLevel() {
  if (auto ast = parseTopLevelExpr()) {
    if (auto* ir = ast->codegen()) {
      std::cerr << "top-level expression ir:\n";
      ir->print(llvm::errs());
      std::cerr << "\n";

      ir->eraseFromParent();
    }
  } else
    skipToNextStatement();
}

int main() {
  std::cout << "herro wolld\n";

  codegenInit();

  std::cout << "ready> " << std::flush;

  loadNextLine();
  getNextToken();

  while (true) {
    switch (token) {
      case token_eof:
        return 0;

      case ';':
        getNextToken();
        break;

      case token_fn:
        handleDef();
        break;

      case token_extern:
        handleExtern();
        break;

      default:
        handleTopLevel();
        break;
    }

    std::cout << "ready> " << std::flush;
  }

  return 0;
}
