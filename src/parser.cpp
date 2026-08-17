#include "kal/parser.hpp"

#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "kal/lexer.hpp"

int token;

static std::map<char, int> precedence = {
    {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};

int getTokenPrecedence() {
  if (!isascii(token)) return -1;

  int prec = precedence[token];

  if (prec <= 0) return -1;
  return prec;
}

int getNextToken() { return token = getToken(); }

std::nullptr_t logError(const char* err) {
  std::cout << "Error: " << err << "\n";
  return nullptr;
}

void skipToNextStatement() {
  while (token != ';' and token != token_eof) getNextToken();
  if (token == ';') getNextToken();
}

std::unique_ptr<AST::Expr> parseExpr() {
  if (auto lhs = parse()) return parseBinaryOpRHS(0, std::move(lhs));
  return nullptr;
}

std::unique_ptr<AST::Expr> parseNumber() {
  auto result = std::make_unique<AST::NumberExpr>(numVal);
  getNextToken();
  return std::move(result);
}

std::unique_ptr<AST::Expr> parseParen() {
  getNextToken();
  auto e = parseExpr();

  if (!e) return nullptr;
  if (token != ')') return logError("expected ')'");

  getNextToken();

  return e;
}

std::unique_ptr<AST::Expr> parseIdent() {
  auto name = identStr;

  getNextToken();

  if (token != '(') return std::make_unique<AST::VariableExpr>(name);

  std::vector<std::unique_ptr<AST::Expr>> args;

  if (getNextToken() != ')') {
    while (true) {
      if (auto arg = parseExpr())
        args.push_back(std::move(arg));
      else
        return nullptr;

      if (token == ')') break;

      if (token != ',') return logError("Expected ')' or ',' in argument list");

      getNextToken();
    }
  }

  getNextToken();

  return std::make_unique<AST::CallExpr>(name, std::move(args));
}

std::unique_ptr<AST::Expr> parseBinaryOpRHS(int exprPrec,
                                            std::unique_ptr<AST::Expr> lhs) {
  while (true) {
    int prec = getTokenPrecedence();

    if (exprPrec > prec) return lhs;

    int op = token;
    getNextToken();

    auto rhs = parse();
    if (!rhs) return nullptr;

    int nextPrec = getTokenPrecedence();
    if (nextPrec > prec) {
      rhs = parseBinaryOpRHS(prec + 1, std::move(rhs));
      if (!rhs) return nullptr;
    }

    lhs = std::make_unique<AST::BinaryExpr>(op, std::move(lhs), std::move(rhs));
  }
}

std::unique_ptr<AST::FnDecl> parseFnDecl() {
  if (token != token_ident)
    return logError("expected function name in prototype");

  std::string fnName = identStr;
  getNextToken();

  if (token != '(') return logError("Expected '(' in function declaration");

  std::vector<std::string> args;

  while (getNextToken() == token_ident) args.push_back(identStr);
  if (token != ')') return logError("expected ')' in function declaration");

  getNextToken();

  return std::make_unique<AST::FnDecl>(fnName, std::move(args));
}

std::unique_ptr<AST::FnDef> parseFnDef() {
  getNextToken();

  auto decl = parseFnDecl();

  if (!decl) return nullptr;

  if (auto e = parseExpr())
    return std::make_unique<AST::FnDef>(std::move(decl), std::move(e));

  return nullptr;
}

std::unique_ptr<AST::FnDecl> parseExtern() {
  getNextToken();
  return parseFnDecl();
}

std::unique_ptr<AST::FnDef> parseTopLevelExpr() {
  if (auto e = parseExpr()) {
    auto decl =
        std::make_unique<AST::FnDecl>("__sus", std::vector<std::string>());
    return std::make_unique<AST::FnDef>(std::move(decl), std::move(e));
  }

  return nullptr;
}

std::unique_ptr<AST::Expr> parse() {
  switch (token) {
    case token_ident:
      return parseIdent();
    case token_num:
      return parseNumber();
    case '(':
      return parseParen();
    default:
      return logError(
          "unknown token encountered while expecting an expression");
  }
}

std::string_view AST::FnDecl::getName() { return name; }
