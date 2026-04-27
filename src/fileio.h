#ifndef ENDIAN_FILEIO_H_
#define ENDIAN_FILEIO_H_

#include <stdio.h>

// Moves all contents of file into a buffer. buf must be freed after use
void fileio_read_all(char** buf, FILE* file);

#endif
