#include <stdio.h>

int tests_run = 0;

void ok(int passed, char *message) {
    if (!passed) {
        printf("not ");
    }
    printf("ok %d - %s\n", ++tests_run, message);
}

int main() {
    ok(1, "truth is true");
    return 0;
}
