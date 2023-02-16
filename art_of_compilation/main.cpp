#include "Compilator.h"
#include <iostream>

int main() {
    Compilator compilator;
    bool test = compilator.Check();
    if (test) {
        std::cout << "OK";
    }
    else {
        std::cout << "\n!!!not OK!!!";
    }
    return 0;
}
