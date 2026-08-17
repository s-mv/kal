#ifndef smv_kal_parser_hpp
#define smv_kal_parser_hpp

#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

extern int token;

namespace AST {

class Expr {
 public:
  virtual ~Expr() = default;
  virtual llvm::Value* codegen() = 0;
};

class NumberExpr : public Expr {
  double val;

 public:
  NumberExpr(double val) : val(val) {}
  llvm::Value* codegen() override;
};

class VariableExpr : public Expr {
  std::string name;

 public:
  VariableExpr(std::string& name) : name(name) {}
  llvm::Value* codegen() override;
};

class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> lhs, rhs;

 public:
  BinaryExpr(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  llvm::Value* codegen() override;
};

class CallExpr : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

 public:
  CallExpr(const std::string& callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {}
  llvm::Value* codegen() override;
};

class FnDecl {
  std::string name;
  std::vector<std::string> args;

 public:
  FnDecl(const std::string& name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {}
  llvm::Function* codegen();
  std::string_view getName();
};

class FnDef {
  std::unique_ptr<FnDecl> decl;
  std::unique_ptr<Expr> body;

 public:
  FnDef(std::unique_ptr<FnDecl> decl, std::unique_ptr<Expr> body)
      : decl(std::move(decl)), body(std::move(body)) {}
  llvm::Function* codegen();
};

}  // namespace AST

int getTokenPrecedence();
int getNextToken();
void skipToNextStatement();

std::nullptr_t logError(const char* err);

std::unique_ptr<AST::Expr> parse();
std::unique_ptr<AST::Expr> parseBinaryOpRHS(int exprPrec,
                                            std::unique_ptr<AST::Expr> lhs);
std::unique_ptr<AST::Expr> parseExpr();
std::unique_ptr<AST::Expr> parseNumber();
std::unique_ptr<AST::Expr> parseParen();
std::unique_ptr<AST::Expr> parseIdent();

std::unique_ptr<AST::FnDecl> parseFnDecl();
std::unique_ptr<AST::FnDef> parseFnDef();
std::unique_ptr<AST::FnDecl> parseExtern();
std::unique_ptr<AST::FnDef> parseTopLevelExpr();

#endif
