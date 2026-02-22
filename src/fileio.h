#ifndef ENDIAN_FILEIO_H_
#define ENDIAN_FILEIO_H_

#include <stdio.h>
#include <stdlib.h>

struct fileio {
  char* buf;
  size_t buf_size;
};

// Moves all contents of file into a buffer
void fileio_read_all(struct fileio* buf, FILE* file);

#endif
