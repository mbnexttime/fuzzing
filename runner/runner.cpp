#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include "llvm/Linker/Linker.h"
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Error.h>
#include "llvm/Support/TargetSelect.h"
#include <iostream>
#include <vector>

// usage 
// pathes to .bc files to link
// name of function to run, etc.
int main(int argc, char* argv[]) {
  if (argc <= 2) {
    return -1;
  }
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::LLVMContext context;
  std::vector<std::string> ir_files;
  for (int i = 1; i < argc - 1; i++) {
    ir_files.push_back(argv[i]);
  }
  std::string fun_name = argv[argc - 1];

  llvm::SMDiagnostic error;
  std::unique_ptr<llvm::Module> llvm_ir_module =
      llvm::parseIRFile(ir_files[0], error, context);
  if (!llvm_ir_module) {
    return -1;
  }

  llvm::Linker linker(*llvm_ir_module);
  for (int i = 1; i < ir_files.size(); i++) {
    std::unique_ptr<llvm::Module> next_ir_module =
      llvm::parseIRFile(ir_files[i], error, context);
    if (!next_ir_module) {
        return -1;
    }
    linker.linkInModule(std::move(next_ir_module));
  }
  std::unique_ptr<llvm::ExecutionEngine> engine(
      llvm::EngineBuilder(std::move(llvm_ir_module))
          .setEngineKind(llvm::EngineKind::JIT)
          .create());
  if (!engine) {
    return -1;
  }

  std::vector<llvm::GenericValue> arguments(2);
  arguments[0].IntVal = llvm::APInt(32, 11);
  arguments[1].IntVal = llvm::APInt(32, 13);

  llvm::Function* function = engine->FindFunctionNamed("_Z3sumii");
  typedef int (*my_function_type)(int, int);
  auto my_function = (my_function_type)(intptr_t)engine->getPointerToFunction(function);
  std::cout << my_function(2, 3) << std::endl;

  return 0;
}
