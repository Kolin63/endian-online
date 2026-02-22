#include "fileio.h"

#include <stdlib.h>
#include <string.h>

void fileio_read_all(struct fileio* buf, FILE* file) {
  char* line = malloc(65536);
  while (fgets(line, sizeof(line), file) != NULL) {
    buf->buf_size += strlen(line);
    buf->buf = realloc(buf->buf, buf->buf_size);
    strcat(buf->buf, line);
  }
  free(line);
}
