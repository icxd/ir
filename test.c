#include "ir.h"
#include <stdio.h>

int main(void) {
  a_init(64 * 1024);
  ir_context context;
  ir_init(&context);

  symbol_index index = ir_find_or_add_symbol(&context, "test");
  printf("%zu\n", index);

  index = ir_find_or_add_symbol(&context, "test");
  printf("%zu\n", index);

  index = ir_find_or_add_symbol(&context, "test2");
  printf("%zu\n", index);

  return 0;
}
