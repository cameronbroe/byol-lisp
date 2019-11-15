#include <stdio.h>
#include "types.h"

lval lval_num(long x) {
    lval value;
    value.type = LVAL_NUM;
    value.num = x;
    return value;
}

lval lval_err(int type) {
    lval err;
    err.type = LVAL_ERR;
    err.err = type;
    return err;
}

lval lval_void() {
    lval value;
    value.type = LVAL_VOID;
    return value;
}

void lval_print(lval value) {
    switch(value.type) {
        case LVAL_NUM:
            printf("%li", value.num);
            break;
        case LVAL_ERR:
            if(value.err == LERR_BAD_OP) {
                fprintf(stderr, "ERROR: Bad operator");
            }
            if(value.err == LERR_BAD_NUM) {
                fprintf(stderr, "ERROR: Bad number");
            }
            if(value.err == LERR_DIV_ZERO) {
                fprintf(stderr, "ERROR: Can't divide by 0, ya silly goose");
            }
            if(value.err == LERR_BAD_CMD) {
                fprintf(stderr, "ERROR: Bad command");
            }
            break;
        default:
            break;
    }
}

void lval_println(lval value) {
    lval_print(value);
    printf("\n");
}
