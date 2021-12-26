#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Returns true on successful read of num_bytes bytes.
// Returns false on EOF.
bool safeRead(int fd, void *buffer, size_t num_bytes) {
  size_t num_bytes_read = 0;

  while (num_bytes_read < num_bytes) {
    size_t n = read(fd, buffer, num_bytes - num_bytes_read);

    if (n == -1) {
      perror("read() failed");
      exit(EXIT_FAILURE);
    }

    if (n == 0) {
      if (num_bytes_read == 0) {
        return false;
      }
      fprintf(stderr, "Unexpected EOF\n");
      exit(EXIT_FAILURE);
    }

    num_bytes_read += n;
    buffer += n;
  }
  return true;
}

void safeWrite(int fd, void *buffer, size_t num_bytes) {
  assert(num_bytes <= PIPE_BUF);

  // Write requests of <= PIPE_BUF bytes shall return nbyte on normal completion.
  if (write(fd, buffer, num_bytes) == -1) {
    perror("write() failed");
    exit(EXIT_FAILURE);
  }
}
