#include "fileio.h"

#include <string.h>

void fileio_read_all(struct fileio* buf, FILE* file) {
  char* line = malloc(65536);
  while (fgets(line, sizeof(line), file) != NULL) {
    buf->buf_size += strlen(line);
    char* new_buf = malloc(buf->buf_size);
    strcpy(new_buf, buf->buf);
    free(buf->buf);
    buf->buf = new_buf;
    strcat(buf->buf, line);
  }
  free(line);
}
