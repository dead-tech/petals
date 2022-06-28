#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdlib.h>

#include "../tokenizer/tokenizer.h"
#include "../utils/utils.h"

#define FILE_CONTENT_CAP 1024 * 1024

#define STACK_CAP 1024

#define STRING_LITERALS_CAP 1024 * 1024
#define STRING_LITERAL_MAX_LENGTH 1024

typedef struct {
  char file_content[FILE_CONTENT_CAP];

  char tokens[TOKEN_MAX_LENGTH + 1][TOKENS_CAP];
  size_t tokens_size;

  int64_t stack[STACK_CAP];
  size_t stack_size;

  char string_literals[STRING_LITERAL_MAX_LENGTH + 1][STRING_LITERALS_CAP];
  size_t string_literals_size;

  size_t ip;
} InterpreterState;


/* Utility */
void state_print_stack(int64_t stack[], size_t stack_size);
void state_print_tokens(char tokens[TOKEN_MAX_LENGTH + 1][TOKENS_CAP], const size_t tokens_size);

/* Methods */
void interpret(InterpreterState *state);

#endif // INTERPRETER_H