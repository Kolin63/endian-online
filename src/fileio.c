#include "fileio.h"

#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 65536

struct fileio* fileio_init() {
  struct fileio* fileio = malloc(sizeof(struct fileio));
  fileio->buf = malloc(1);
  fileio->buf[0] = '\0';
  fileio->buf_size = 1;
  return fileio;
}

void fileio_cleanup(struct fileio* buf) {
  free(buf->buf);
  free(buf);
}

void fileio_read_all(struct fileio* buf, FILE* file) {
  char* line = malloc(LINE_BUF_SIZE);
  while (fgets(line, LINE_BUF_SIZE, file) != NULL) {
    buf->buf_size += strlen(line);
    buf->buf = realloc(buf->buf, buf->buf_size);
    strcat(buf->buf, line);
  }
  free(line);
}
