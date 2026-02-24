#include "fileio.h"

#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 65536

void fileio_read_all(struct fileio* buf, FILE* file) {
  char* line = malloc(LINE_BUF_SIZE);
  while (fgets(line, LINE_BUF_SIZE, file) != NULL) {
    buf->buf_size += strlen(line);
    buf->buf = realloc(buf->buf, buf->buf_size);
    strcat(buf->buf, line);
  }
  free(line);
}
