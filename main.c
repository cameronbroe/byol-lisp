#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <stdlib.h>
#include <editline/readline.h>
#endif

#define PROGRAM_NAME "byol-lisp"
#define VERSION "0.0.1.dev"

#ifdef _WIN32
// Input buffer for REPL
static char input[2048]; // 2 KB buffer
#endif

void __print_version();

int main(int argc, char** argv) {
    __print_version();
    while(1) {
#ifdef _WIN32
        printf("%s> ", PROGRAM_NAME);
        fgets(input, 2048, stdin);
        int len = strlen(input);
        *(input + (len - 2)) = '\0';
#else
        char prompt[66];
        sprintf(prompt, "%s> ", PROGRAM_NAME);
        char* input = readline(prompt);

        add_history(input);
#endif


        if(strstr(input, "!quit") != NULL) {
            break;
        }
        printf("The hell is a %s\n", input);

#ifndef _WIN32
        free(input);
#endif
    }
    return 0;
}

void __print_version() {
    printf("%s ver. %s\n", PROGRAM_NAME, VERSION);
    printf("Ctrl+C or !quit to Exit\n");
}
