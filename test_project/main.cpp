#include <iostream>

int mult(int a, int b);

int sum(int a, int b) {
    return mult(a + 2, b + 2);
}

int main() {
    sum(3, 4);
    return 0;
}