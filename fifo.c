#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct
{
  char name[20];
  int fd;
} fifo_t;

// Returns true on successful creation
// Returns false if fifo with specified name already exists.
bool createFifo(fifo_t *fifo, char *name) {
  sprintf(fifo->name, "%s_%d", name, getppid());
  if (mkfifo(fifo->name, S_IRUSR | S_IWUSR) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo() failed");
      exit(EXIT_FAILURE);
    }
    return false;
  }
  return true;
}

void openFifo(fifo_t *fifo, int mode) {
  fifo->fd = open(fifo->name, mode);
  if (fifo->fd == -1) {
    perror("open() failed");
    exit(EXIT_FAILURE);
  }
}

void closeFifo(fifo_t *fifo) {
  if (close(fifo->fd) == -1) {
    perror("close() failed");
    exit(EXIT_FAILURE);
  }
}

void unlinkFifo(fifo_t *fifo) {
  if (unlink(fifo->name) == -1) {
    perror("unlink() failed");
    exit(EXIT_FAILURE);
  }
}
