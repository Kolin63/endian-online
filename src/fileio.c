#include "fileio.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 65536

void fileio_read_all(char** buf, FILE* file) {
  assert(*buf == NULL);
  char* buf_ = *buf;
  buf_ = malloc(1);
  buf_[0] = '\0';
  size_t buf_size = 1;
  char* line = malloc(LINE_BUF_SIZE);
  while (fgets(line, LINE_BUF_SIZE, file) != NULL) {
    buf_size += strlen(line);
    buf_ = realloc(buf_, buf_size);
    strcat(buf_, line);
  }
  free(line);
  *buf = buf_;
}
