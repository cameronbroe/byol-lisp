#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <editline/readline.h>
#include <stdlib.h>
#endif

#define PROGRAM_NAME "byol-lisp"
#define VERSION "0.0.7-dev"

#ifdef _WIN32
// Input buffer for REPL
static char input[2048]; // 2 KB buffer
#endif

#include "evaluators.h"
#include "mpc.h"
#include "types.h"

void __print_version();

int main(int argc, char **argv) {
  __print_version();

  // Create parsers
  mpc_parser_t *cmd_p = mpc_new("cmd");
  mpc_parser_t *number_p = mpc_new("number");
  mpc_parser_t *symbol_p = mpc_new("symbol");
  mpc_parser_t *sexpr_p = mpc_new("sexpr");
  mpc_parser_t *expr_p = mpc_new("expr");
  mpc_parser_t *prog_p = mpc_new("prog");

  // Define grammar

  // Define language
  mpca_lang_contents(MPCA_LANG_DEFAULT, "./grammar.mpc", cmd_p, number_p,
                     symbol_p, sexpr_p, expr_p, prog_p);
  while (1) {
#ifdef _WIN32
    printf("%s> ", PROGRAM_NAME);
    fgets(input, 2048, stdin);
    int len = strlen(input);
    input[strcspn(input, "\r\n")] = '\0';
#else
    char prompt[66];
    sprintf(prompt, "%s> ", PROGRAM_NAME);
    char *input = readline(prompt);

    add_history(input);
#endif

    // Parse the input
    mpc_result_t result;
    if (mpc_parse("<stdin>", input, prog_p, &result)) {
      lval *root = lval_read(result.output);
      root = lval_eval(root);
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
