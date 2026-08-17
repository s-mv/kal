#ifndef smv_kal_codegen_hpp
#define smv_kal_codegen_hpp

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <string>

static std::unique_ptr<llvm::LLVMContext> ctx;
static std::unique_ptr<llvm::IRBuilder<>> builder;
static std::unique_ptr<llvm::Module> module;
static std::unordered_map<std::string, llvm::Value*> nameValueMap;

void codegenInit();

#endif
