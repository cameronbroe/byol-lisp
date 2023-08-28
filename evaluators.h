#ifndef __EVALUATORS_H__
#define __EVALUATORS_H__

#include "types.h"

lval *lval_eval_sexpr(lval *expr);
lval *lval_eval(lval *value);
lval *lval_pop(lval *value, int i);
lval *lval_take(lval *value, int i);

lval *evaluate_symbol(lval *value, char *symbol);
lval *evaluate_command(char *cmd);

#endif
