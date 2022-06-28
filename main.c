#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define ENABLE_TRACING false

#define FILE_CONTENT_CAP 1024 * 1024

#define TOKEN_MAX_LENGTH 64
#define TOKENS_CAP 1024

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

void slice_str(const char *str, char *buffer, size_t start, size_t end)
{
  size_t j = 0;
  for (size_t i = start; i <= end; ++i) {
    buffer[j++] = str[i];
  }
  buffer[j] = 0;
}

bool is_number(const char* word)
{
  size_t word_len = strlen(word);
  for (size_t i = 0; i < word_len; ++i) {
    if (!isdigit(word[i])) {
      return false;
    }
  }
  return true;
}

void print_usage()
{
  printf("./petals <file>\n");
}

void print_stack(int64_t stack[], size_t stack_size)
{
  printf("[");
  for (size_t i = 0; i < stack_size; ++i) {
    printf(" %zu,", stack[i]);
  }
  printf(" ]\n");
}

void print_tokens(char tokens[TOKEN_MAX_LENGTH + 1][TOKENS_CAP], const size_t tokens_size)
{
  printf("{");
  for (size_t i = 0; i < tokens_size; ++i) {
    printf(" %s,", tokens[i]);
  }
  printf(" }\n");
}

size_t read_file(const char* file_path, char file_content[])
{
  FILE* handle = fopen(file_path, "rb");

  if (handle == NULL) {
    fprintf(stderr, "petals error: could not read file for reason: %s\n", strerror(errno));
    exit(1);
  }

  // Get the file size
  fseek(handle, 0, SEEK_END);
  size_t file_size = ftell(handle);
  rewind(handle);

  size_t read_size = fread(file_content, sizeof(char), file_size, handle);
  fclose(handle);

  if (file_size != read_size) {
    fprintf(stderr, "petals error: unable to read the whole file for reason: %s\n", strerror(errno));
    exit(1);
  }

  return file_size;
}

void parse_word(char *it, char word[TOKEN_MAX_LENGTH])
{
  size_t i = 0;
  for (char *ch = it; *ch != ' '; ++ch) {
    const size_t size = strlen(ch);
    if (size < 1) {
      break;
    }

    if (i > (TOKEN_MAX_LENGTH - 2)) {
      fprintf(stderr, "petals error: token exceeded max token length = %d\n", TOKEN_MAX_LENGTH);
      exit(1);
    }

    word[i++] = *ch;
  }
}

// @param it - Basically the pointer to the first double quote in the string literal
void parse_string_literal(char *it, char string_literal[TOKEN_MAX_LENGTH])
{
  size_t i = 0;
  string_literal[i++] = *it;

  for (char *ch = ++it; *ch != '"'; ++ch) {
    if (i > (TOKEN_MAX_LENGTH - 2)) {
      fprintf(stderr, "petals error: could not find closing double quote\n");
      exit(1);
    }
    string_literal[i++] = *ch;
  }

  string_literal[i++] = '"';
}

size_t tokenize(char* file_content, char tokens[][TOKENS_CAP]) {
  size_t tokens_size = 0;

  for (char *it = file_content; *it != '\0';) {
    if (*it == '"') {
      char string_literal[TOKEN_MAX_LENGTH] = {0};
      parse_string_literal(it, string_literal);
      const size_t string_literal_size = strlen(string_literal);

      strcpy(tokens[tokens_size++], string_literal);

      // Here we skip the quoted string 
      it += string_literal_size + 1;
      continue;
    }

    char word[TOKEN_MAX_LENGTH] = {0};
    parse_word(it, word);

    strcpy(tokens[tokens_size++], word);
    it += strlen(word) + 1;
  }

  return tokens_size;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "petals error: no input file was provided\n");
    print_usage();
    exit(1);
  }

  static InterpreterState state = {0};

  read_file(argv[1], state.file_content);
  state.tokens_size = tokenize(state.file_content, state.tokens);

  for (size_t i = 0; i < state.tokens_size; ++i) {
    char* word = state.tokens[state.ip];

#if ENABLE_TRACING
    print_tokens(tokens, tokens_size);
    print_stack(stack, stack_size);
#endif

    if (is_number(word)) {
      state.stack[state.stack_size++] = atoi(word);
      ++state.ip;
    } else if (strcmp("+", word) == 0) {
      int64_t a = state.stack[--state.stack_size];
      int64_t b = state.stack[--state.stack_size];
      state.stack[state.stack_size++] = a + b;
      ++state.ip;
    } else if (strcmp("puts", word) == 0) {
      int64_t index = state.stack[--state.stack_size];
      printf("%s", state.string_literals[index]);
      --state.string_literals_size;
      ++state.ip;
    } else if (strcmp("putsln", word) == 0) {
      int64_t index = state.stack[--state.stack_size];
      printf("%s\n", state.string_literals[index]);
      --state.string_literals_size;
      ++state.ip;
    } else if (strcmp("print", word) == 0) {
      int64_t top = state.stack[--state.stack_size];
      printf("%zu", top);
      ++state.ip;
    } else if (strcmp("println", word) == 0) {
      int64_t top = state.stack[--state.stack_size];
      printf("%zu\n", top);
      ++state.ip;
    } else if (strcmp("swap", word) == 0) {
      int64_t a = state.stack[--state.stack_size];
      int64_t b = state.stack[--state.stack_size];
      state.stack[state.stack_size++] = a;
      state.stack[state.stack_size++] = b;
      ++state.ip;
    } else if (strcmp("drop", word) == 0) {
      __attribute__((unused)) int64_t a = state.stack[--state.stack_size];
      ++state.ip;
    } else if (strncmp(word, "\"", 1) == 0) {
      size_t word_size = strlen(word);

      char quoted_word[word_size - 2];
      slice_str(word, quoted_word, 1, word_size - 2);

      strcpy(state.string_literals[state.string_literals_size++], quoted_word);

      state.stack[state.stack_size++] = word_size - 2;
      state.stack[state.stack_size++] = state.string_literals_size - 1;
      ++state.ip;
    }
  }

  if (state.stack_size > 0) {
    fprintf(stderr, "petals error: unhandled data on the stack\n");
    exit(1);
  }

  return 0;
}
