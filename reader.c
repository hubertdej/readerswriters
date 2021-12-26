#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "fifo.c"
#include "safeio.c"

int main(int argc, char *argv[]) {
  assert(sizeof(int) == 4);

  fifo_t fifo;
  createFifo(&fifo, "MAIN");
  openFifo(&fifo, O_RDONLY);

  int value;
  while (safeRead(fifo.fd, &value, 4)) {
    printf("%d\n", value);
  }
  closeFifo(&fifo);
}
