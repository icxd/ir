#include "ir.h"
#include <stdio.h>

int main(void) {
  a_init(64 * 1024);
  ir_context context;
  ir_init(&context);
  /*
   *    def w @add(w %a, w %b) {
   *    bb1:
   *      %1 = add w %a, w %b
   *      ret w %1
   *    }
   *
   *    def w @main() {
   *    entry:
   *      %1 = call w @add(w 50, w 100)
   *      ret w %1
   *    }
   */

  ir_function add_fn = {
      .return_type = IR_TYPE_WORD,
      .name_index = ir_add_symbol(&context, "add"),
  };
  ir_slice_init(&add_fn.blocks);

  ir_block bb1 = {ir_add_symbol(&context, "bb1")};
  ir_slice_init(&bb1.instructions);

  ir_slice_push(&bb1.instructions, (ir_instruction){
                                       .tag = _IR_INSTRUCTION_ADD,
                                       .ADD = {},
                                   });

  ir_slice_push(&add_fn.blocks, bb1);

  // symbol_index index = ir_find_or_add_symbol(&context, "test");
  // printf("%zu\n", index);

  // index = ir_find_or_add_symbol(&context, "test");
  // printf("%zu\n", index);

  // index = ir_find_or_add_symbol(&context, "test2");
  // printf("%zu\n", index);

  return 0;
}
