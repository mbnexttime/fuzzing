папка с репозиторием должна стоять в корневой папке пользователя
0. в пакете pass: clang -emit-llvm -c -fno-use-cxa-atexit logop.cpp -o logop.ll
1. в пакете pass: cmake ./
2. в пакете pass: make
3. в пакете runner: cmake ./
4. в пакете runner: make
5. в пакете test_project: cmake ./
6. в пакете test_project: make
7. в пакете test_project: python3.10 ./../cmake_tools/modifier.py compile_commands.json llvm_files_list.txt new_commands.json ~/example/pass/libAstPass.so ~/example/pass/logop.ll
8. в пакете test_project: python3.10 ./../cmake_tools/runner.py new_commands.json 
9. в пакете test_project: ./../runner/runner _Z3sumii 6004 int32 llvm_files_list.txt 
10. в пакете test_project: java -jar ./../../fuzzing.jar --portCpp 6004 --functionDescriptionPath description.txt