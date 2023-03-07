0. cmake-ом собираем .so файл
1. Создаём <name>.cpp файл
2. clang -emit-llvm -c <name>.cpp -o <name>.bc
3. opt -load ~/example/pass/libAstPass.so -ast -enable-new-pm=0 test.bc > modified_test.bc
4. lli modified_test.bc