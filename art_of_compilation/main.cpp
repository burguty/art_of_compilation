#include "Compilator.h"

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
