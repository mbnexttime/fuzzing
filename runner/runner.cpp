#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include "llvm/IR/Function.h"
#include <llvm/ProfileData/Coverage/CoverageMapping.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include "llvm/Linker/Linker.h"
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Error.h>
#include "llvm/Support/TargetSelect.h"
#include <string>  
#include <iostream>
#include <fstream>
#include <sstream>  
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
using namespace llvm;


std::vector<std::string> split_buffer_by_newline(const char* buffer, size_t buffer_size) {
    std::vector<std::string> lines;
    std::stringstream ss(std::string(buffer, buffer_size - 1));

    std::string line;
    while (std::getline(ss, line, ';')) {
        lines.push_back(line);
    }

    return lines;
}

llvm::GenericValue parseArguments(std::string& value) {
  llvm::GenericValue llvmValue;
  if (value.substr(0, 7) == "int32: ") {
    llvmValue.IntVal = llvm::APInt(32, std::stoi(value.substr(7)));
  } else if (value.substr(0, 6) == "bool: ") {
    llvmValue.IntVal = (value.substr(6) == "true" ? llvm::APInt(1, 1) : llvm::APInt(1, 0));
  }

  return llvmValue;
}

string decode_argument(char* type, llvm::GenericValue& value) {
  if (strcmp(type, "int32") == 0) {
    return "int32: " + to_string(value.IntVal.getSExtValue());
  }
  if (strcmp(type, "bool") == 0) {
    string temp = "bool: ";
    temp += (value.IntVal.getSExtValue() == 0 ? "false" : "true");
    return temp;
  }
  return "crash";
}

int main(int argc, char* argv[]) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::LLVMContext context;
  ifstream llvm_ir_files(argv[4], ios_base::in);
  string temp_file_path;
  std::vector<std::string> ir_files;
  while (llvm_ir_files >> temp_file_path) {
    ir_files.push_back(temp_file_path);
  }

  llvm::SMDiagnostic error;
  std::unique_ptr<llvm::Module> llvm_ir_module =
      llvm::parseIRFile(ir_files[0], error, context);
  if (!llvm_ir_module) {
    cerr << "Error creating first ir file";
    return -1;
  }

  llvm::Linker linker(*llvm_ir_module);
  for (int i = 1; i < ir_files.size(); i++) {
    std::unique_ptr<llvm::Module> next_ir_module =
      llvm::parseIRFile(ir_files[i], error, context);
    if (!next_ir_module) {
      cerr << "Error creating i-th ir file";
      return -1;
    }
    linker.linkInModule(std::move(next_ir_module));
  }
  std::unique_ptr<llvm::ExecutionEngine> engine(
      llvm::EngineBuilder(std::move(llvm_ir_module))
          .setEngineKind(llvm::EngineKind::Interpreter)
          .create());
  if (!engine) {  
    cerr << "Error creating engine";
    return -1;
  }

  llvm::Function* function = engine->FindFunctionNamed(argv[1]);

  if (function == nullptr) {
    cerr << "Error finding function to run";
    return -1;
  }

  int portCPP = std::atoi(argv[2]);

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    cerr << "Error creating receiver socket";
    return -1;
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(portCPP);
  
  if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
    cerr << "Error binding socket to port сpp" << endl;
    close(server_socket);
    return -1;
  }

  char buffer[8196];
  
  while (true) {
    
    if (listen(server_socket, SOMAXCONN) == -1) {
      std::cerr << "Error listening on port cpp" << std::endl;
      close(server_socket);
      return -1;
    }
    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
      std::cerr << "Error accepting incoming connection on port cpp" << std::endl;
      close(server_socket);
      return -1;
    }
    while (true) {
      int num_bytes_received = read(client_socket, buffer, sizeof(buffer));
      if (num_bytes_received == -1) {
          std::cerr << "Error reading data on port cpp" << std::endl;
          close(client_socket);
          break;
      }
      if (num_bytes_received == 0) {
        break;
      }

      auto arguments = split_buffer_by_newline(buffer, num_bytes_received);
      std::vector<llvm::GenericValue> llvmArguments;

      for (auto& argument: arguments) {
          llvmArguments.push_back(parseArguments(argument));
      }
      std::stringstream traces;
      std::cout.rdbuf(traces.rdbuf()); 
      auto result = engine->runFunction(function, llvmArguments);
      fflush(stdout);
      auto output = decode_argument(argv[3], result);
      output = output + ";" + traces.str() + '\n';
      if (send(client_socket, output.c_str(), output.length(), 0) == -1) {
        std::cerr << "Failed to send data to jvm";
        close(client_socket);
        break;
      }
    }
  }
  // _Z3sumii
  return 0;
}
