#include "ir.h"
#include <stdio.h>

int main(void) {
  a_init(64 * 1024);
  ir_state state;
  ir_init(&state);

  symbol_index index = ir_find_or_add_symbol(&state, "test");
  printf("%zu\n", index);

  index = ir_find_or_add_symbol(&state, "test");
  printf("%zu\n", index);

  index = ir_find_or_add_symbol(&state, "test2");
  printf("%zu\n", index);

  return 0;
}
