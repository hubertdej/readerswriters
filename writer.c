#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#include "fifo.c"
#include "safeio.c"

#define INPUT_FD_1 3
#define INPUT_FD_2 4

int main(int argc, char *argv[]) {
  assert(sizeof(int) == 4);
  assert(argc >= 2);

  const int num_writers = atoi(argv[1]);

  fifo_t fifo;
  createFifo(&fifo, "MAIN");
  openFifo(&fifo, O_WRONLY);

  fd_set producer_fds;
  FD_ZERO(&producer_fds);
  FD_SET(INPUT_FD_1, &producer_fds);
  FD_SET(INPUT_FD_2, &producer_fds);

  int fd_bound = ((INPUT_FD_1 > INPUT_FD_2) ? INPUT_FD_1 : INPUT_FD_2) + 1;
  int fd;

  while (true) {
    fd_set ready_fds = producer_fds;
    if (select(fd_bound, &ready_fds, NULL, NULL, NULL) == -1) {
      perror("select() failed");
      exit(EXIT_FAILURE);
    }

    fd = FD_ISSET(INPUT_FD_1, &ready_fds) ? INPUT_FD_1 : INPUT_FD_2;

    int value;
    if (!safeRead(fd, &value, 4)) {
      fd = (fd == INPUT_FD_1) ? INPUT_FD_2 : INPUT_FD_1;
      break;
    }

    safeWrite(fifo.fd, &value, 4);
  }

  int value;
  while (safeRead(fd, &value, 4)) {
    safeWrite(fifo.fd, &value, 4);
  }

  fifo_t ready_fifo;
  if (!createFifo(&ready_fifo, "READY")) {
    openFifo(&ready_fifo, O_WRONLY);
    safeWrite(ready_fifo.fd, "\1", 1);
    closeFifo(&fifo);
    closeFifo(&ready_fifo);
    return EXIT_SUCCESS;
  }

  openFifo(&ready_fifo, O_RDWR);

  for (int i = 1; i < num_writers; ++i) {
    char c;
    safeRead(ready_fifo.fd, &c, 1);
  }

  closeFifo(&fifo);
  closeFifo(&ready_fifo);
  unlinkFifo(&fifo);
  unlinkFifo(&ready_fifo);
  return EXIT_SUCCESS;
}
