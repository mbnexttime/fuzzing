#include <iostream>

int mult(int a, int b);

int sum(int a, int b) {
    mult(a, b);
    return a + b;
}

int main() {
    sum(3, 4);
    return 0;
}