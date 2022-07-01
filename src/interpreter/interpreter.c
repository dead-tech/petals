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
  if (getenv("TRACE")) {
    state_print_tokens(state->tokens, state->tokens_size);
  }

  while (state->ip < state->tokens_size) {
    char* word = state->tokens[state->ip];

    if (getenv("TRACE")) {
      printf("\nCurrent word: %s\t IP: %zu\n", word, state->ip);
    }

    if (is_number(word)) {
      state->stack[state->stack_size++] = atoi(word);
      ++state->ip;
    } else if (strcmp("+", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a + b;
      ++state->ip;
    } else if (strcmp("-", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = b - a;
      ++state->ip;
    } else if (strcmp("=", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = b == a;
      ++state->ip;
    } else if (strcmp("<", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = b < a;
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
      assert(state->stack_size >= 2 && "petals error: not enough arguments for swap");
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = b;
      ++state->ip;
    } else if (strcmp("pairswap", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      int64_t c = state->stack[--state->stack_size];
      int64_t d = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = b;
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = d;
      state->stack[state->stack_size++] = c;
      ++state->ip;
    } else if (strcmp("over", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = b;
      state->stack[state->stack_size++] = a;
      ++state->ip;
    } else if (strcmp("dup", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = a;
      ++state->ip;
    } else if (strcmp("2swap", word) == 0) {
      int64_t a = state->stack[--state->stack_size];
      int64_t b = state->stack[--state->stack_size];
      int64_t c = state->stack[--state->stack_size];
      state->stack[state->stack_size++] = a;
      state->stack[state->stack_size++] = b;
      state->stack[state->stack_size++] = c;
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
    } else if (strcmp("while", word) == 0) {
      state->stack[state->stack_size++] = state->ip;
      state->current_statement_type = WHILE_STATEMENT;
      ++state->ip;
    } else if (strcmp("if", word) == 0) {
      int64_t condition = state->stack[--state->stack_size];
      if (condition == true) {
        ++state->ip;
      } else {
        while (strcmp("end", state->tokens[state->ip]) != 0) {
          ++state->ip;
        }
      }
      state->current_statement_type = IF_STATEMENT;
    } else if (strcmp("do", word) == 0) {
      int64_t condition = state->stack[--state->stack_size];
      // TODO: Clean this if statement
      if (condition == true) {
        ++state->ip;
      } else if (condition == false) {
        // TODO: Handle
        while (strcmp("end", state->tokens[state->ip]) != 0) {
          ++state->ip;
        }
      }
      state->stack[state->stack_size++] = condition;
    } else if (strcmp("end", word) == 0) {
      if (state->current_statement_type == IF_STATEMENT) {
        state->current_statement_type = NONE;
        ++state->ip;
        continue;
      }

      int64_t condition = state->stack[--state->stack_size];
      if (!condition) {
        ++state->ip;
      } else {
        int64_t while_addr = state->stack[--state->stack_size];
        state->ip = while_addr;
      }

      state->current_statement_type = NONE;
    }

    if (getenv("TRACE")) {
      // state_print_tokens(state->tokens, state->tokens_size);
      printf("\n");
      state_print_stack(state->stack, state->stack_size);
    }

  }
}