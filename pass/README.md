0. cmake-ом собираем .so файл
1. Создаём <name>.cpp файл (далее <name> = test) или вносим изменения в test.cpp
2. clang -emit-llvm -c -fno-use-cxa-atexit test.cpp -o test.ll
3. opt -load ~/example/pass/libAstPass.so -ast -enable-new-pm=0 test.ll > modified_test.ll
4. clang -emit-llvm -c -fno-use-cxa-atexit logop.cpp -o logop.ll
5. llvm-link logop.ll modified_test.ll -o result.bc
6. (optional) lli result.bc (чтобы этот шаг работал, нужно убрать -fno-use-cxa-atexit. Этот шаг просто для демонстрации работы, для работы ранера нужно все делать с флагом)

можно воспользоваться уже собранными result.bc (для раннера) и lli_result.bc (для lli lli_result.bc)