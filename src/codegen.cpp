#include "kal/codegen.hpp"

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include <memory>
#include <string>
#include <vector>

#include "kal/parser.hpp"

void codegenInit() {
  ctx = std::make_unique<llvm::LLVMContext>();
  module = std::make_unique<llvm::Module>("smv", *ctx);
  builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
}

llvm::Value* AST::NumberExpr::codegen() {
  return llvm::ConstantFP::get(*ctx, llvm::APFloat(val));
}

llvm::Value* AST::VariableExpr::codegen() {
  llvm::Value* v = nameValueMap[name];
  if (!v) return logError("unknown varable name");
  return v;
}

llvm::Value* AST::BinaryExpr::codegen() {
  llvm::Value* l = lhs->codegen();
  llvm::Value* r = rhs->codegen();

  if (!l or !r) return nullptr;

  switch (op) {
    case '+':
      return builder->CreateFAdd(l, r, "addtmp");
    case '-':
      return builder->CreateFSub(l, r, "subtmp");
    case '*':
      return builder->CreateFMul(l, r, "multmp");
    case '<':
      l = builder->CreateFCmpULT(l, r, "cmptmp");
      return builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*ctx), "booltmp");
    default:
      return logError("invalid binary operator :(");
  }
}

llvm::Value* AST::CallExpr::codegen() {
  llvm::Function* calleeFn = module->getFunction(callee);

  if (!calleeFn) return logError("unknown function called");

  if (calleeFn->arg_size() != args.size())
    return logError("incorrect number of arguments to function");

  std::vector<llvm::Value*> args_llvm;

  for (int i = 0; i < args.size(); i++) {
    args_llvm.push_back(args[i]->codegen());
    if (!args_llvm.back()) return nullptr;
  }

  return builder->CreateCall(calleeFn, args_llvm, "calltmp");
}

llvm::Function* AST::FnDecl::codegen() {
  std::vector<llvm::Type*> doubles(args.size(), llvm::Type::getDoubleTy(*ctx));

  llvm::FunctionType* fnType =
      llvm::FunctionType::get(llvm::Type::getDoubleTy(*ctx), doubles, false);
  llvm::Function* fn = llvm::Function::Create(
      fnType, llvm::Function::ExternalLinkage, name, module.get());

  int i = 0;

  for (auto& arg : fn->args()) {
    arg.setName(args[i++]);
  }

  return fn;
}

llvm::Function* AST::FnDef::codegen() {
  llvm::Function* fn = module->getFunction(decl->getName());

  if (!fn) fn = decl->codegen();

  if (!fn) return nullptr;

  if (!fn->empty()) return logError("a function cannot be redefined!");

  llvm::BasicBlock* bb = llvm::BasicBlock::Create(*ctx, "entry", fn);
  builder->SetInsertPoint(bb);

  nameValueMap.clear();

  for (auto& arg : fn->args()) nameValueMap[std::string(arg.getName())] = &arg;

  if (llvm::Value* returnVal = body->codegen()) {
    builder->CreateRet(returnVal);
    llvm::verifyFunction(*fn);
    return fn;
  }

  fn->eraseFromParent();
  return nullptr;
}
