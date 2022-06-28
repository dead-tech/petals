#include "utils.h"

void slice_str(const char *str, char *buffer, const size_t start, const size_t end)
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