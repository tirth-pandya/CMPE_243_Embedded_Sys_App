#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t queue_memory[100];
  uint8_t head_index;
  uint8_t tail_index;
  size_t item_count;
} queue_s;

// Initialize all members of queue_s
void queue__init(queue_s *queue);

//@returns false if queue is full
bool queue__push(queue_s *queue, uint8_t push_value);

//@returns false if queue was empty
bool queue__pop(queue_s *queue, uint8_t *pop_value);

size_t queue__get_item_count(const queue_s *queue);