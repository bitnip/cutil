#include "assertion.h"

int asserts_passed = 0;
int asserts_failed = 0;

void stringTest();

int main() {
    stringTest();

    printf("Asserts Passed: %d, Failed: %d\n",
        asserts_passed, asserts_failed);
    return asserts_failed;
}
