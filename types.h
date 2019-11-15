#ifndef __TYPES_H__
#define __TYPES_H__

typedef struct {
    int type;
    long num;
    int err;
} lval;

// lval types
enum { LVAL_NUM, LVAL_ERR, LVAL_VOID };

// err types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_BAD_CMD };

// type functions
lval lval_num(long x);
lval lval_err(int type);
lval lval_void();
void lval_print(lval value);
void lval_println(lval value);

#endif
