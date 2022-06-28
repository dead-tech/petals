#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "src/utils/utils.h"

#include "src/tokenizer/tokenizer.h"
#include "src/interpreter/interpreter.h"

void print_usage()
{
  printf("./petals <file>\n");
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
  interpret(&state);

  if (state.stack_size > 0) {
    fprintf(stderr, "petals error: unhandled data on the stack\n");
    exit(1);
  }

  return 0;
}
