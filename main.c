#include <stdio.h>
#include <string.h>

#define PROGRAM_NAME "byol-lisp"
#define VERSION "0.0.1.dev"

// Input buffer for REPL
static char input[2048]; // 2 KB buffer

void __print_version();

int main(int argc, char** argv) {
    __print_version();
    while(1) {
        printf("%s> ", PROGRAM_NAME);
        fgets(input, 2048, stdin);

        if(strstr(input, "!quit") != NULL) {
            break;
        }
        printf("The hell is a %s", input);
    }
    return 0;
}

void __print_version() {
    printf("%s ver. %s\n", PROGRAM_NAME, VERSION);
    printf("Ctrl+C or !quit to Exit\n");
}
