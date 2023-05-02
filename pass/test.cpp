#include <iostream>

int sum(int a, int b) {
    if (a > 5) {
        int temp = 0;
        if (b > 5) {
            return temp - a;
        } else {
            return a + b;
        }
        return temp;
    } else {
        return a - b;
    }
}

int main() {
    sum(3, 4);
    return 0;
}