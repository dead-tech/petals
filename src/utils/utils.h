#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* String */
void slice_str(const char *str, char *buffer, const size_t start, const size_t end);
bool is_number(const char* word);

/* Filesystem */
size_t read_file(const char* file_path, char file_content[]);

#endif // UTILS_H