#ifndef __TYPES_H__
#define __TYPES_H__

#include "mpc.h"

typedef struct _lval {
    int type;
    long num;
    // String types for error and symbols
    char* err;
    char* sym;

    // String type for command
    char* cmd;

    // Count and list of lval*
    int count;
    struct _lval** cell;
} lval;

// lval types
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_CMD, LVAL_VOID };

// err types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_BAD_CMD };

// type functions
lval* lval_num(long x);
lval* lval_err(char* message);
lval* lval_sym(char* symbol);
lval* lval_sexpr();
lval* lval_cmd(char* cmd);
lval* lval_void();

void lval_print(lval* value);
void lval_println(lval* value);
void lval_expr_print(lval* node, char open, char close);

void lval_del(lval* value);
lval* lval_add(lval* value, lval* new_lval);
lval* lval_read(mpc_ast_t* node);
lval* lval_read_num(mpc_ast_t* node);

#endif
