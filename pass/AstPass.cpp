#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <cstdlib>
#include <iostream>


using namespace llvm;

namespace {
struct Ast : public FunctionPass {
  static char ID;
  Ast() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    int counter = 0;
    
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        // Get a reference to the module's LLVM context
        LLVMContext &Context = F.getContext();

        // Get a pointer to the printf function
        Type *VoidTy = Type::getVoidTy(Context);
        Type *Int32Ty = Type::getInt32Ty(Context);
        Type *Int8PtrTy = Type::getInt8PtrTy(Context);
        FunctionType *PrintfTy = FunctionType::get(VoidTy, {Int8PtrTy, Int8PtrTy}, false);
        FunctionCallee PrintfFunc = F.getParent()->getOrInsertFunction("_Z5logopPcS_", PrintfTy);

        // Create a constant string containing the function name
        Constant *FuncName = ConstantDataArray::getString(Context, F.getName());
        GlobalVariable *StrVar = new GlobalVariable(
          *F.getParent(), FuncName->getType(), true,
          GlobalValue::PrivateLinkage, FuncName, ".str");

        Constant *InstructionStr = ConstantDataArray::getString(Context, std::to_string(counter));
        GlobalVariable *InstructionStrVar = new GlobalVariable(
          *F.getParent(), InstructionStr->getType(), true,
          GlobalValue::PrivateLinkage, InstructionStr, ".str");

        Value *StrPtr = ConstantExpr::getBitCast(StrVar, Int8PtrTy);
        Value *InstructionPtr = ConstantExpr::getBitCast(InstructionStrVar, Int8PtrTy);
        IRBuilder<> Builder(&I);
        std::vector<Value*> Args;
        Args.push_back(StrPtr);
        Args.push_back(InstructionPtr);
        Builder.CreateCall(PrintfFunc, Args);

        counter++;
      }
    }
    return true;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char Ast::ID = 0;
static RegisterPass<Ast> X("ast", "ast", false /* Only looks at CFG */, false /* Analysis Pass */);