#ifndef IR_H
#define IR_H

#include <stdint.h>
#include <stdlib.h>

/*
 * Like my previous implementation of this, I don't really have any ideas of
 * what I'm doing.. I just kind of have an idea of what I want as an end
 * result.
 *
 *    def w @add(w %a, w %b) {
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

typedef struct arena {
  struct arena *next;
  size_t capacity, size;
  uint8_t *data;
} arena_t;

void _a_init(size_t);
void *a_alloc(size_t);
void *a_realloc(void *, size_t, size_t);
void a_reset();
void a_free();

#define a_init(size)                                                           \
  do {                                                                         \
    _a_init(size);                                                             \
    atexit(a_free);                                                            \
  } while (0)

#define INIT_SLICE_CAP 16

/// A dynamically sized array-like data structure.
#define ir_slice(T)                                                            \
  struct {                                                                     \
    T *items;                                                                  \
    size_t capacity, size;                                                     \
  }
#define ir_slice_init(s)                                                       \
  do {                                                                         \
    (s)->items = (__typeof__(*(s)->items) *)a_alloc(sizeof(*(s)->items) *      \
                                                    INIT_SLICE_CAP);           \
    (s)->capacity = INIT_SLICE_CAP;                                            \
    (s)->size = 0;                                                             \
  } while (0)
#define ir_slice_push(s, ...)                                                  \
  do {                                                                         \
    if ((s)->size >= (s)->capacity) {                                          \
      size_t old_size = (s)->capacity;                                         \
      (s)->capacity = (s)->capacity == 0 ? INIT_SLICE_CAP : (s)->capacity * 2; \
      (s)->items = (__typeof__(*(s)->items) *)a_realloc(                       \
          (s)->items, old_size, sizeof(*(s)->items) * (s)->capacity);          \
    }                                                                          \
    (s)->items[(s)->size++] = (__VA_ARGS__);                                   \
  } while (0)

typedef const char *cstr;
/*
 * This is required for all `ir_slice` instances due to not being able
 * to assign anonymous structures to eachother even though the structures
 * and the exact same size, fields, etc... This may not be useful for the
 * implementation, but it's useful to have incase the user wants to assign
 * a slice to another slice, we simply define it for them here so that
 * the user doesn't have to define it themselves.
 */
typedef ir_slice(cstr) cstr_slice;
typedef size_t symbol_index;

typedef enum {
  _IR_TYPE_WORD,
  _IR_TYPE_POINTER,
} ir_type_tag;
typedef struct ir_type {
  ir_type_tag tag;
  union {
    struct ir_type *ptr;
  };
} ir_type;
#define IR_TYPE_WORD ((ir_type){_IR_TYPE_WORD})
#define IR_TYPE_POINTER(T) ((ir_type){.tag = _IR_TYPE_POINTER, .ptr = (&(T))})

typedef enum {
  _IR_VALUE_INTEGER,
  _IR_VALUE_SYMBOL,
} ir_value_tag;
typedef struct {
  ir_value_tag tag;
  ir_type type;
  union {
    int64_t integer;
    symbol_index symbol;
  };
} ir_value;
#define IR_VALUE_INTEGER(T, V)                                                 \
  ((ir_value){.tag = _IR_VALUE_INTEGER, .type = (T), .integer = (V)})
#define IR_VALUE_SYMBOL(T, V)                                                  \
  ((ir_value){.tag = _IR_VALUE_SYMBOL, .type = (T), .symbol = (V)})

typedef enum {
#define INST_RET(id, ...) _IR_INSTRUCTION_##id,
#define INST_NORET(id, ...) _IR_INSTRUCTION_##id,
#include "instructions.inc"
#undef INST_RET
#undef INST_NORET
} ir_instruction_tag;
typedef struct {
  ir_instruction_tag tag;
  union {
#define INST_RET(id, ...)                                                      \
  struct {                                                                     \
    size_t index;                                                              \
    __VA_ARGS__                                                                \
  } id;
#define INST_NORET(id, ...)                                                    \
  struct {                                                                     \
    __VA_ARGS__                                                                \
  } id;
#include "instructions.inc"
#undef INST_RET
#undef INST_NORET
  };
} ir_instruction;
typedef ir_slice(ir_instruction) ir_instruction_slice;

typedef struct {
  symbol_index name_index;
  ir_instruction_slice instructions;
} ir_block;
typedef ir_slice(ir_block) ir_block_slice;

typedef struct {
  ir_type return_type;
  symbol_index name_index;
  // TODO: parameters
  ir_block_slice blocks;
} ir_function;

typedef struct {
  cstr_slice symbol_table;
} ir_context;

void ir_init(ir_context *);
symbol_index ir_find_or_add_symbol(ir_context *, cstr);
/// Returns SIZE_MAX if symbol wasn't found.
symbol_index ir_find_symbol(ir_context *, cstr);
symbol_index ir_add_symbol(ir_context *, cstr);

#endif // IR_H
