#include "evaluators.h"


lval* lval_eval_sexpr(lval* expr) {
    // Iterate and evaluate children
    for(int i = 0; i < expr->count; i++) {
        expr->cell[i] = lval_eval(expr->cell[i]);
    }

    // Check for errors
    for(int i = 0; i < expr->count; i++) {
        if(expr->cell[i]->type == LVAL_ERR) {
            return lval_take(expr, i);
        }
    }

    // Handle empty expression
    if(expr->count == 0) {
        return expr;
    }

    // Handle single expression
    if(expr->count == 1) {
        return lval_take(expr, 0);
    }

    // Validate first element is a symbol
    lval* first = lval_pop(expr, 0);
    if(first->type != LVAL_SYM) {
        lval_del(first);
        lval_del(expr);
        return lval_err("S-expression should start with symbol");
    }

    // Evaluate symbol
    lval* result = evaluate_symbol(expr, first->sym);
    lval_del(first);
    return result;
}

lval* lval_eval(lval* value) {
    // If s-expr, evaluate it
    if(value->type == LVAL_SEXPR) {
        return lval_eval_sexpr(value);
    } else if(value->type == LVAL_CMD) {
        return evaluate_command(value->cmd);
    }
    return value;
}

lval* lval_pop(lval* value, int i) {
    lval* x = value->cell[i];

    // Move memory after i up
    memmove(&value->cell[i], &value->cell[i+1], sizeof(lval*) * (value->count - i - 1));

    // Decrement count of list
    value->count--;

    // Reallocate memory
    value->cell = realloc(value->cell, sizeof(lval*) * value->count);
    return x;
}

lval* lval_take(lval* value, int i) {
    lval* x = lval_pop(x, i);
    lval_del(value);
    return x;
}

lval* evaluate_symbol(lval* value, char* symbol) {
    // Make sure arguments are numbers
    for(int i = 0; i < value->count; i++) {
        if(value->cell[i]->type != LVAL_NUM) {
            lval_del(value);
            return lval_err("Arguments for symbol must be integers");
        }
    }

    // Get first element
    lval* x = lval_pop(value, 0);

    // If there are no arguments and symbol is subtraction, perform unary negation
    if(strcmp(symbol, "-") == 0 && value->count == 0) {
        x->num = -1 * x->num;
    }

    // Process rest of arguments
    while(value->count > 0) {
        // Get next element
        lval* y = lval_pop(value, 0);

        if(strcmp(symbol, "+") == 0) {
            x->num = x->num + y->num;
        } else if(strcmp(symbol, "-") == 0) {
            x->num = x->num - y->num;
        } else if(strcmp(symbol, "*") == 0) {
            x->num = x->num * y->num;
        } else if(strcmp(symbol, "/") == 0) {
            if(y->num == 0) {
               lval_del(x);
               lval_del(y);
               x = lval_err("can't divide by zero");
               break;
            }
            x->num = x->num / y->num;
        } else if(strcmp(symbol, "%") == 0) {
            if(y->num == 0) {
               lval_del(x);
               lval_del(y);
               x = lval_err("can't divide by zero");
               break;
            }
            x->num = x->num % y->num;
        } else if(strcmp(symbol, "^") == 0) {
            x->num = pow(x->num, y->num);
        } else if(strcmp(symbol, "min") == 0) {
            x->num = x->num < y->num ? x->num : y->num;
        } else if(strcmp(symbol, "max") == 0) {
            x->num = x->num > y->num ? x->num : y->num;
        }

        lval_del(y);
    }

    lval_del(value);
    return x;
}

lval* evaluate_command(char* cmd) {
    if(strcmp(cmd, "quit") == 0) {
        exit(0);
        return lval_void();
    }
    return lval_err("bad command");
}
