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

#include "mpc.h"


void __print_version();

int main(int argc, char** argv) {
    __print_version();

    // Create parsers
    mpc_parser_t* number = mpc_new("number");
    mpc_parser_t* operator = mpc_new("operator");
    mpc_parser_t* expr = mpc_new("expr");
    mpc_parser_t* prog = mpc_new("prog");

    // Define grammar
    char* polish_grammar = " \
    number : /-?[0-9]+/ ; \
    operator : '+' | '-' | '*' | '/' ; \
    expr : <number> | '(' <operator> <expr>+ ')' ; \
    prog : /^/ <operator> <expr>+ /$/ ; \
    ";

    // Define language
    mpca_lang(MPCA_LANG_DEFAULT,
            polish_grammar,
            number,
            operator,
            expr,
            prog);
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
    mpc_cleanup(4, number, operator, expr, prog);
    return 0;
}

void __print_version() {
    printf("%s ver. %s\n", PROGRAM_NAME, VERSION);
    printf("Ctrl+C or !quit to Exit\n");
}
