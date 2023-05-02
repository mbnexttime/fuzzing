#include <iostream>

void logop(char* funcName, char* instruction_number) {
    std::cout << "Function called: " + std::string(funcName) + ", instruction: " << std::string(instruction_number) << ";";
}