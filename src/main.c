#define PORT 6969

#include "webc.h"

int main(void) {
  webc_init();
  webc_start(PORT);
}

