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
#include "types.h"


void __print_version();
lval* evaluate_program(mpc_ast_t* node);
lval* evaluate_operator(char* operator, lval* x, lval* y);
lval* evaluate_command(char* cmd);

int main(int argc, char** argv) {
    __print_version();

    // Create parsers
    mpc_parser_t* cmd_p = mpc_new("cmd");
    mpc_parser_t* number_p = mpc_new("number");
    mpc_parser_t* symbol_p = mpc_new("symbol");
    mpc_parser_t* sexpr_p = mpc_new("sexpr");
    mpc_parser_t* expr_p = mpc_new("expr");
    mpc_parser_t* prog_p = mpc_new("prog");

    // Define grammar

    // Define language
    mpca_lang_contents(MPCA_LANG_DEFAULT,
            "./grammar.mpc",
            cmd_p,
            number_p,
            symbol_p,
            sexpr_p,
            expr_p,
            prog_p);
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
        if(mpc_parse("<stdin>", input, prog_p, &result)) {
            lval* root = lval_read(result.output);
            lval_println(root);
            lval_del(root);
            /* lval output = evaluate_program(result.output); */
            /* lval_println(output); */
            mpc_ast_delete(result.output);
        } else {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }

#ifndef _WIN32
        free(input);
#endif
    }
    mpc_cleanup(5, cmd_p, number_p, symbol_p, sexpr_p, expr_p, prog_p);
    return 0;
}

void __print_version() {
    printf("%s ver. %s\n", PROGRAM_NAME, VERSION);
    printf("Ctrl+C or !quit to Exit\n");
}

lval* evaluate_program(mpc_ast_t* node) {
    // Implement commands
    if(strstr(node->tag, "cmd") != NULL) {
        return evaluate_command(node->contents);
    }

    // Numbers should just be converted
    if(strstr(node->tag, "number") != NULL) {
        errno = 0;
        long num = strtol(node->contents, NULL, 10);
        return errno != ERANGE ? lval_num(num) : lval_err("bad number");
    }

    // Check operator in second child
    char* operator = node->children[1]->contents;

    // Next child is an expression, evaluate it and store it in a variable
    lval* x = evaluate_program(node->children[2]);

    // Iterate the rest of the expressions
    for(int i = 3; strstr(node->children[i]->tag, "expr") != NULL; i++) {
        x = evaluate_operator(operator, x, evaluate_program(node->children[i]));
    }

    return x;
}

lval* evaluate_operator(char* operator, lval* x, lval* y) {
    lval* return_value = lval_err("bad operator");

    // If a value is an err, just return it
    if(x->type == LVAL_ERR) {
        return_value = x;
        goto force_return;
    }
    if(y->type == LVAL_ERR) {
        return_value = y;
        goto force_return;
    }

    if(strcmp(operator, "+") == 0) {
        return_value = lval_num(x->num + y->num);
    } else if(strcmp(operator, "-") == 0) {
        return_value = lval_num(x->num - y->num);
    } else if(strcmp(operator, "*") == 0) {
        return_value = lval_num(x->num * y->num);
    } else if(strcmp(operator, "/") == 0) {
        if(y->num == 0) {
           return_value = lval_err("can't divide by zero");
        } else {
           return_value = lval_num(x->num / y->num);
        }
    } else if(strcmp(operator, "%") == 0) {
        return_value = lval_num(x->num % y->num);
    } else if(strcmp(operator, "^") == 0) {
        return_value = lval_num(pow(x->num, y->num));
    } else if(strcmp(operator, "min") == 0) {
        return_value = x->num < y->num ? x : y;
    } else if(strcmp(operator, "max") == 0) {
        return_value = x->num > y->num ? x : y;
    }

force_return:
    return return_value;
}

lval* evaluate_command(char* cmd) {
    if(strcmp(cmd, "quit") == 0) {
        exit(0);
        return lval_void();
    }
    return lval_err("bad command");
}
