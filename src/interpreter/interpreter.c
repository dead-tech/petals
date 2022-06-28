#include "interpreter.h"

void state_print_stack(int64_t stack[], size_t stack_size)
{
  printf("[");
  for (size_t i = 0; i < stack_size; ++i) {
    printf(" %zu,", stack[i]);
  }
  printf(" ]\n");
}

void state_print_tokens(char tokens[TOKEN_MAX_LENGTH + 1][TOKENS_CAP], const size_t tokens_size)
{
  printf("{");
  for (size_t i = 0; i < tokens_size; ++i) {
    printf(" %s,", tokens[i]);
  }
  printf(" }\n");
}

void interpret(InterpreterState *state)
{
  for (size_t i = 0; i < state->tokens_size; ++i) {
    char* word = state->tokens[state->ip];

    if (getenv("TRACE")) {
      state_print_tokens(state->tokens, state->tokens_size);
      state_print_stack(state->stack, state->stack_size);
    }

    if (is_number(word)) {
      state->stack[state->stack_size++] = atoi(word);
      ++state->ip;
    } else if (strcmp("+", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a + b;
      ++state->ip;
    } else if (strcmp("puts", word) == 0) {
      int64_t index = state->stack[--state->stack_size];
      printf("%s", state->string_literals[index]);
      --state->string_literals_size;
      ++state->ip;
    } else if (strcmp("putsln", word) == 0) {
      int64_t index = state->stack[--state->stack_size];
      printf("%s\n", state->string_literals[index]);
      --state->string_literals_size;
      ++state->ip;
    } else if (strcmp("print", word) == 0) {
      int64_t top = state->stack[--state->stack_size];
      printf("%zu", top);
      ++state->ip;
    } else if (strcmp("println", word) == 0) {
      int64_t top = state->stack[--state->stack_size];
      printf("%zu\n", top);
      ++state->ip;
    } else if (strcmp("swap", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = b;
      ++state->ip;
    } else if (strcmp("drop", word) == 0) {
      __attribute__((unused)) int64_t a = state->stack[--state->stack_size];
      ++state->ip;
    } else if (strncmp(word, "\"", 1) == 0) {
      size_t word_size = strlen(word);

      char quoted_word[word_size - 2];
      slice_str(word, quoted_word, 1, word_size - 2);

      strcpy(state->string_literals[state->string_literals_size++], quoted_word);

      state->stack[state->stack_size++] = word_size - 2;
      state->stack[state->stack_size++] = state->string_literals_size - 1;
      ++state->ip;
    }
  }
}