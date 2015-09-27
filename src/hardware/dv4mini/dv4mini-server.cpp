#include <stdlib.h>
#include <stdio.h>
#include "../../lib/platform/platform.h"
#include "../../lib/dv4mini/dv4mini.h"

int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf("Syntax: %s <devicefile>\n", argv[0]);
    return 1;
  }

  DV4Mini mini;
  if(!mini.open(argv[1])) {
    perror(argv[1]);
    return 1;
  }
  printf("Using device at %s.\n", argv[1]);
  sleep(10);
  mini.close();
}
