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
long evaluate_program(mpc_ast_t* node);

int main(int argc, char** argv) {
    __print_version();

    // Create parsers
    mpc_parser_t* cmd = mpc_new("cmd");
    mpc_parser_t* number = mpc_new("number");
    mpc_parser_t* operator = mpc_new("operator");
    mpc_parser_t* expr = mpc_new("expr");
    mpc_parser_t* prog = mpc_new("prog");

    // Define grammar
    char* polish_grammar = " \
    cmd : /[a-z]+/ ; \
    number : /-?[0-9]+/ ; \
    operator : '+' | '-' | '*' | '/' | '%' ; \
    expr : <number> | '(' <operator> <expr>+ ')' ; \
    prog : /^/ <operator> <expr>+ /$/ \
         | /^/ '!' <cmd> /$/ ; \
    ";

    // Define language
    mpca_lang(MPCA_LANG_DEFAULT,
            polish_grammar,
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
        *(input + (len - 2)) = '\0';
#else
        char prompt[66];
        sprintf(prompt, "%s> ", PROGRAM_NAME);
        char* input = readline(prompt);

        add_history(input);
#endif

        // Parse the input
        mpc_result_t result;
        if(mpc_parse("<stdin>", input, prog, &result)) {
            long number_of_nodes = evaluate_program(result.output);
            printf("Your expression has %ld nodes\n", number_of_nodes);
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
    printf("tag: %s\ncontents: %s\n\n", node->tag, node->contents);
    if(node->children_num == 0) {
        return 1;
    } else if(node->children_num > 0) {
        long total = 1;
        for(int i = 0; i < node->children_num; i++) {
            total += evaluate_program(node->children[i]);
        }
        return total;
    } else {
        return 0;
    }
}
