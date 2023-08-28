#include "types.h"
#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval *lval_num(long x) {
  lval *value = (lval *)malloc(sizeof(lval));
  value->type = LVAL_NUM;
  value->num = x;
  return value;
}

lval *lval_err(char *message) {
  lval *err = (lval *)malloc(sizeof(lval));
  err->type = LVAL_ERR;
  err->err = malloc(strlen(message) + 1); // stupid NULL-terminated strings
  strcpy(err->err, message);
  return err;
}

lval *lval_sym(char *symbol) {
  lval *value = (lval *)malloc(sizeof(lval));
  value->type = LVAL_SYM;
  value->sym = malloc(strlen(symbol) + 1);
  strcpy(value->sym, symbol);
  return value;
}

lval *lval_sexpr() {
  lval *sexpr = (lval *)malloc(sizeof(lval));
  sexpr->type = LVAL_SEXPR;
  sexpr->count = 0;
  sexpr->cell = NULL;
  return sexpr;
}

lval *lval_cmd(char *cmd) {
  lval *value = (lval *)malloc(sizeof(lval));
  value->type = LVAL_CMD;
  value->cmd = malloc(strlen(cmd) + 1);
  strcpy(value->cmd, cmd);
  return value;
}

lval *lval_void() {
  lval *value = (lval *)malloc(sizeof(lval));
  value->type = LVAL_VOID;
  return value;
}

void lval_print(lval *value) {
  switch (value->type) {
  case LVAL_NUM:
    printf("%li", value->num);
    break;
  case LVAL_ERR:
    fprintf(stderr, "ERROR: %s", value->err);
    break;
  case LVAL_SYM:
    printf("%s", value->sym);
    break;
  case LVAL_SEXPR:
    lval_expr_print(value, '(', ')');
    break;
  case LVAL_CMD:
    printf("!%s", value->cmd);
    break;
  default:
    break;
  }
}

void lval_expr_print(lval *node, char open, char close) {
  printf("%c", open);
  for (int i = 0; i < node->count; i++) {
    // Print the value in the node
    lval_print(node->cell[i]);

    // Print a trailing space if not last element
    if (i != (node->count - 1)) {
      printf(" ");
    }
  }
  printf("%c", close);
}

void lval_del(lval *value) {
  switch (value->type) {
  case LVAL_NUM:
    break;
  case LVAL_ERR:
    free(value->err);
    break;
  case LVAL_SYM:
    free(value->sym);
    break;
  case LVAL_CMD:
    free(value->cmd);
    break;
  case LVAL_SEXPR:
    for (int i = 0; i < value->count; i++) {
      lval_del(value->cell[i]);
    }
    free(value->cell);
    break;
  }

  free(value);
}

void lval_println(lval *value) {
  lval_print(value);
  printf("\n");
}

lval *lval_add(lval *value, lval *new_lval) {
  value->count++;
  value->cell = (lval **)realloc(value->cell, sizeof(lval *) * value->count);
  value->cell[value->count - 1] = new_lval;
  return value;
}

lval *lval_read(mpc_ast_t *node) {
  if (strstr(node->tag, "number")) {
    return lval_read_num(node);
  }
  if (strstr(node->tag, "symbol")) {
    return lval_sym(node->contents);
  }

  if (strstr(node->tag, "cmd")) {
    return lval_cmd(node->contents);
  }

  lval *list = NULL;
  if (strcmp(node->tag, ">") == 0) {
    list = lval_sexpr();
  }
  if (strstr(node->tag, "sexpr")) {
    list = lval_sexpr();
  }

  for (int i = 0; i < node->children_num; i++) {
    if (strcmp(node->children[i]->contents, "(") == 0) {
      continue;
    }
    if (strcmp(node->children[i]->contents, ")") == 0) {
      continue;
    }
    if (strcmp(node->children[i]->tag, "regex") == 0) {
      continue;
    }
    if (strcmp(node->children[i]->tag, "char") == 0) {
      continue;
    }
    list = lval_add(list, lval_read(node->children[i]));
  }

  return list;
}

lval *lval_read_num(mpc_ast_t *node) {
  errno = 0;
  long num = strtol(node->contents, NULL, 10);
  return errno != ERANGE ? lval_num(num) : lval_err("invalid number");
}
