#include "util.h"

#include <stdlib.h>

bool read_file(const char *filename, size_t *out_size, char **out_data) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "failed to open file.\n");
    return false;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  if (size < 1) {
    fclose(fp);
    return false;
  }
  rewind(fp);

  char *data = malloc(size + 1);
  if (!data) {
    fclose(fp);
    fprintf(stderr, "failed to allocate memory for file.\n");
    return false;
  }
  data[size] = '\0';

  size_t size_read = fread(data, 1, size, fp);
  if (size != (long)size_read) {
    fclose(fp);
    free(data);
    fprintf(stderr, "failed to fully read file.\n");
    return false;
  }

  *out_size = size_read + 1;
  *out_data = data;

  fclose(fp);

  return true;
}
