#include "ir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static arena_t *get_local_arena() {
  static arena_t arena = {NULL, 0, 0, NULL};
  return &arena;
}

static void *a_malloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    perror("malloc() failed");
    exit(-1);
  }
  return ptr;
}

void _a_init(size_t capacity) {
  arena_t *a = get_local_arena();
  if (a->data == NULL) {
    a->data = (uint8_t *)a_malloc(capacity);
    a->capacity = capacity;
  }
}

void *a_alloc(size_t size) {
  arena_t *a = get_local_arena(), *curr = a;
  assert(curr->capacity >= size);

  while (!(curr->size + size <= curr->capacity)) {
    if (curr->next == NULL) {
      arena_t *next = (arena_t *)a_malloc(sizeof(arena_t));
      next->capacity = a->capacity;
      next->size = 0;
      next->next = NULL;
      next->data = (uint8_t *)a_malloc(a->capacity);
      curr->next = next;
    }
    curr = curr->next;
  }

  uint8_t *data = &curr->data[curr->size];
  curr->size += size;
  return data;
}

void *a_realloc(void *old_ptr, size_t old_size, size_t new_size) {
  if (new_size <= old_size)
    return old_ptr;
  void *new_ptr = a_alloc(new_size);
  char *new_ptr_char = (char *)new_ptr, *old_ptr_char = (char *)old_ptr;
  for (size_t i = 0; i < old_size; ++i)
    new_ptr_char[i] = old_ptr_char[i];
  return new_ptr;
}

void a_reset() {
  arena_t *curr = get_local_arena();
  while (curr != NULL) {
    curr->size = 0;
    curr = curr->next;
  }
}

void a_free() {
  arena_t *a = get_local_arena();
  free(a->data);
  a->capacity = 0;
  a->size = 0;
  arena_t *curr = a->next;
  while (curr != NULL) {
    arena_t *tmp = curr->next;
    free(curr->data);
    free(curr);
    curr = tmp;
  }
  a->next = NULL;
}

void ir_init(ir_context *context) { ir_slice_init(&context->symbol_table); }

symbol_index ir_find_or_add_symbol(ir_context *context, cstr symbol) {
  symbol_index index = ir_find_symbol(context, symbol);
  if (index == SIZE_MAX) {
    ir_slice_push(&context->symbol_table, symbol);
    index = context->symbol_table.size - 1;
  }
  return index;
}

symbol_index ir_find_symbol(ir_context *context, cstr symbol) {
  symbol_index index = SIZE_MAX;
  for (size_t i = 0; i < context->symbol_table.size; i++) {
    cstr found_symbol = context->symbol_table.items[i];
    if (strncmp(symbol, found_symbol, strlen(symbol)) == 0) {
      index = i;
    }
  }
  return index;
}

symbol_index ir_add_symbol(ir_context *context, cstr symbol) {
  if (ir_find_symbol(context, symbol) != SIZE_MAX)
    return SIZE_MAX;
  ir_slice_push(&context->symbol_table, symbol);
  return context->symbol_table.size - 1;
}
