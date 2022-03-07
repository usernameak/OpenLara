#pragma once

#include <AEEStdLib.h>
#include <AEEFile.h>

#define FILE IFile

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

FILE *bfio_fopen(const char *fname, const char *mode);
int bfio_fclose(FILE *fp);
size_t bfio_fwrite(const void *buf, size_t size, size_t count, FILE *stream);
size_t bfio_fread (      void *buf, size_t size, size_t count, FILE *stream);
long bfio_ftell(FILE *stream);
int bfio_fseek (FILE *stream, long offset, int origin);
void bfio_init(IShell *pIShell);
void bfio_dispose();

#define fopen bfio_fopen
#define fclose bfio_fclose
#define fwrite bfio_fwrite
#define fread bfio_fread
#define fseek bfio_fseek
#define ftell bfio_ftell
