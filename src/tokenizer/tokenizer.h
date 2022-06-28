#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_MAX_LENGTH 64
#define TOKENS_CAP 1024

void parse_word(char *it, char word[TOKEN_MAX_LENGTH]);
void parse_string_literal(char *it, char string_literal[TOKEN_MAX_LENGTH]);
size_t tokenize(char* file_content, char tokens[][TOKENS_CAP]);

#endif // TOKENIZER_H