#include "tokenizer.h"

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

size_t tokenize(char* file_content, char tokens[][TOKENS_CAP])
{
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