#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


using namespace llvm;

namespace {
struct Ast : public FunctionPass {
  static char ID;
  Ast() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    // Get a reference to the module's LLVM context
    LLVMContext &Context = F.getContext();

    // Get a pointer to the printf function
    Type *VoidTy = Type::getVoidTy(Context);
    Type *Int32Ty = Type::getInt32Ty(Context);
    Type *Int8PtrTy = Type::getInt8PtrTy(Context);
    FunctionType *PrintfTy = FunctionType::get(Int32Ty, {Int8PtrTy}, true);
    FunctionCallee PrintfFunc = F.getParent()->getOrInsertFunction("printf", PrintfTy);

    // Create a constant string containing the function name
    Constant *FuncName = ConstantDataArray::getString(Context, F.getName());
    GlobalVariable *StrVar = new GlobalVariable(
      *F.getParent(), FuncName->getType(), true,
      GlobalValue::PrivateLinkage, FuncName, ".str");

    // Build the format string for the printf call
    Constant *FormatStr = ConstantDataArray::getString(Context, "Function called: %s\n");
    GlobalVariable *FormatStrVar = new GlobalVariable(
      *F.getParent(), FormatStr->getType(), true,
      GlobalValue::PrivateLinkage, FormatStr, ".str");

    // Get pointers to the format string and function name
    Value *FormatStrPtr = ConstantExpr::getBitCast(FormatStrVar, Int8PtrTy);
    Value *StrPtr = ConstantExpr::getBitCast(StrVar, Int8PtrTy);

    // Insert the printf call at the beginning of the function
    IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());
    std::vector<Value*> Args;
    Args.push_back(FormatStrPtr);
    Args.push_back(StrPtr);
    Builder.CreateCall(PrintfFunc, Args);

    return true;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char Ast::ID = 0;
static RegisterPass<Ast> X("ast", "ast", false /* Only looks at CFG */, false /* Analysis Pass */);