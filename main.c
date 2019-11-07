#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
#include <stdlib.h>
#include <editline/readline.h>
#endif

#define PROGRAM_NAME "byol-lisp"
#define VERSION "0.0.7-dev"

#ifdef _WIN32
// Input buffer for REPL
static char input[2048]; // 2 KB buffer
#endif

#include "mpc.h"


void __print_version();
long evaluate_program(mpc_ast_t* node);
long evaluate_operator(char* operator, long x, long y);
void evaluate_command(char* cmd);

int main(int argc, char** argv) {
    __print_version();

    // Create parsers
    mpc_parser_t* cmd = mpc_new("cmd");
    mpc_parser_t* number = mpc_new("number");
    mpc_parser_t* operator = mpc_new("operator");
    mpc_parser_t* expr = mpc_new("expr");
    mpc_parser_t* prog = mpc_new("prog");

    // Define grammar

    // Define language
    mpca_lang_contents(MPCA_LANG_DEFAULT,
            "./grammar.mpc",
            cmd,
            number,
            operator,
            expr,
            prog);
    while(1) {
#ifdef _WIN32
        printf("%s> ", PROGRAM_NAME);
        fgets(input, 2048, stdin);
        int len = strlen(input);
        input[strcspn(input, "\r\n")] = '\0';
#else
        char prompt[66];
        sprintf(prompt, "%s> ", PROGRAM_NAME);
        char* input = readline(prompt);

        add_history(input);
#endif

        // Parse the input
        mpc_result_t result;
        if(mpc_parse("<stdin>", input, prog, &result)) {
            long output = evaluate_program(result.output);
            printf("%s = %ld\n", input, output);
            mpc_ast_delete(result.output);
        } else {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }

#ifndef _WIN32
        free(input);
#endif
    }
    mpc_cleanup(5, cmd, number, operator, expr, prog);
    return 0;
}

void __print_version() {
    printf("%s ver. %s\n", PROGRAM_NAME, VERSION);
    printf("Ctrl+C or !quit to Exit\n");
}

long evaluate_program(mpc_ast_t* node) {
    // Implement commands
    if(strstr(node->tag, "cmd") != NULL) {
        evaluate_command(node->contents);
        return 0;
    }

    // Numbers should just be converted
    if(strstr(node->tag, "number") != NULL) {
        return atol(node->contents);
    }

    // Check operator in second child
    char* operator = node->children[1]->contents;

    // Next child is an expression, evaluate it and store it in a variable
    long x = evaluate_program(node->children[2]);

    // Iterate the rest of the expressions
    for(int i = 3; strstr(node->children[i]->tag, "expr") != NULL; i++) {
        x = evaluate_operator(operator, x, evaluate_program(node->children[i]));
    }

    return x;
}

long evaluate_operator(char* operator, long x, long y) {
    if(strcmp(operator, "+") == 0) {
        return x + y;
    } else if(strcmp(operator, "-") == 0) {
        return x - y;
    } else if(strcmp(operator, "*") == 0) {
        return x * y;
    } else if(strcmp(operator, "/") == 0) {
        return x / y;
    } else if(strcmp(operator, "%") == 0) {
        return x % y;
    } else if(strcmp(operator, "^") == 0) {
        return pow(x, y);
    } else if(strcmp(operator, "min") == 0) {
        return x < y ? x : y;
    } else if(strcmp(operator, "max") == 0) {
        return x > y ? x : y;
    } else {
        return 0;
    }
}

void evaluate_command(char* cmd) {
    if(strcmp(cmd, "quit") == 0) {
        exit(0);
    }
}
