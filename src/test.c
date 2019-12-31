#include "assertion.h"

int asserts_passed = 0;
int asserts_failed = 0;

void stringTest();
void listTest();
void vectorTest();
void mapTest();
void genericTest();
void resourceTest();
void uriTest();

int main() {
    stringTest();
    listTest();
    vectorTest();
    mapTest();
    genericTest();
    uriTest();
    resourceTest();

    printf("Asserts Passed: %d, Failed: %d\n",
        asserts_passed, asserts_failed);
    return asserts_failed;
}
