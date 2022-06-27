#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define FILE_CONTENT_CAP 1024 * 1024

#define TOKEN_MAX_LENGTH 64
#define TOKENS_CAP 1024

#define STACK_CAP 1024

#define STRING_LITERALS_CAP 1024 * 1024
#define STRING_LITERAL_MAX_LENGTH 1024

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

size_t tokenize(const char* file_content, char tokens[][TOKENS_CAP]) {
  size_t tokens_size = 0;
  const char* delimiter = " \n";

  // This is needed to save the state of strtok
  // This way we are able to manually shift the next position, therefore where
  // to start searching the next delimiter
  char *save_ptr;

  // We need to make a copy of the wole file_content as strtok will modify it
  // by replacing the delimiters with \0
  char file_content_copy[FILE_CONTENT_CAP] = {0};
  char *it = &file_content_copy[0];
  strcpy(it, file_content);

  bool starts_with_string_literal = *file_content == '"';

  // If we were to call strtok even if the file starts with a string literal
  // it would not parse that string literal correctly as strtok skips the first
  // delimiter.
  if (!starts_with_string_literal) {
    it = strtok_r(it, delimiter, &save_ptr);
  }

  while (it != NULL) {
    if (*it == '"') {
      char string_literal[TOKEN_MAX_LENGTH] = {0};
      parse_string_literal(it, string_literal);
      const size_t string_literal_size = strlen(string_literal);

      strcpy(tokens[tokens_size++], string_literal);

      // Here we skip the quoted string and then save the current position of
      // the iterator to the state of strtok so that the next delimiter is
      // searched starting from after the last double quote
      // We also call to strtok to effectively shift the pointer to this location
      it += string_literal_size + 1;
      save_ptr = it;
      it = strtok_r(NULL, delimiter, &save_ptr);
      continue;
    }

    strcpy(tokens[tokens_size++], it);
    it = strtok_r(NULL, delimiter, &save_ptr);
  }

  return tokens_size;
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

int main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "petals error: no input file was provided\n");
    print_usage();
    exit(1);
  }

  static char file_content[FILE_CONTENT_CAP] = {0};
  read_file(argv[1], file_content);

  static char tokens[TOKEN_MAX_LENGTH + 1][TOKENS_CAP] = {0};
  size_t tokens_size = tokenize(file_content, tokens);

  static int64_t stack[STACK_CAP] = {0};
  size_t stack_size = 0;

  static char string_literals[STRING_LITERAL_MAX_LENGTH + 1][STRING_LITERALS_CAP] = {0};
  size_t string_literals_size = 0;

  size_t ip = 0;

  for (size_t i = 0; i < tokens_size; ++i) {
    char* word = tokens[ip];

    if (is_number(word)) {
      stack[stack_size++] = atoi(word);
      ++ip;
    } else if (strcmp("+", word) == 0) {
      int64_t a = stack[--stack_size];
      int64_t b = stack[--stack_size];
      stack[stack_size++] = a + b;
      ++ip;
    } else if (strcmp("puts", word) == 0) {
      int64_t index = stack[--stack_size];
      printf("%s\n", string_literals[index]);
      --string_literals_size;
      ++ip;
    } else if (strcmp("print", word) == 0) {
      int64_t top = stack[--stack_size];
      printf("%zu\n", top);
      ++ip;
    } else if (strcmp("swap", word) == 0) {
      int64_t a = stack[--stack_size];
      int64_t b = stack[--stack_size];
      stack[stack_size++] = a;
      stack[stack_size++] = b;
      ++ip;
    } else if (strcmp("drop", word) == 0) {
      __attribute__((unused)) int64_t a = stack[--stack_size];
      ++ip;
    } else if (strncmp(word, "\"", 1) == 0) {
      size_t word_size = strlen(word);

      char quoted_word[word_size - 2];
      slice_str(word, quoted_word, 1, word_size - 2);

      strcpy(string_literals[string_literals_size++], quoted_word);

      stack[stack_size++] = word_size - 2;
      stack[stack_size++] = string_literals_size - 1;
      ++ip;
    }
  }


  return 0;
}
