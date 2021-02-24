#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *static_memory_for_queue;
  size_t static_memory_size_in_bytes;
  uint8_t head_index;
  uint8_t tail_index;
  size_t item_count;
} queue_s;

/* Initialize the queue with user provided static memory
 * @param static_memory_for_queue This memory pointer should not go out of scope
 *
 * @code
 *   static uint8_t memory[128];
 *   queue_s queue;
 *   queue__init(&queue, memory, sizeof(memory));
 * @endcode
 */

void s_queue__init(queue_s *queue, void *static_memory_for_queue, size_t static_memory_size_in_bytes);

/// @returns false if the queue is full
bool s_queue__push(queue_s *queue, uint8_t push_value);

/// @returns false if the queue was empty
/// Write the popped value to the user provided pointer pop_value_ptr
bool s_queue__pop(queue_s *queue, uint8_t *pop_value_ptr);

size_t s_queue__get_item_count(const queue_s *queue);