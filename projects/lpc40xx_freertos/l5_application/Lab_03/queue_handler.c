#include "queue_handler.h"

void queue__init(queue_s *queue) {
  queue->head_index = 0;
  queue->tail_index = 0;
  queue->item_count = 0;
}

bool queue__push(queue_s *queue, uint8_t push_value) {
  uint8_t max_queue_size = 100;
  if (queue->item_count < max_queue_size) {
    queue->queue_memory[queue->head_index] = push_value;
    queue->head_index = (queue->head_index + 1) % max_queue_size;
    queue->item_count++;
    return true;
  } else
    return false;
}

bool queue__pop(queue_s *queue, uint8_t *pop_value) {
  uint8_t max_queue_size = 100;
  if (queue->item_count > 0) {
    *pop_value = queue->queue_memory[queue->tail_index];
    queue->tail_index = (queue->tail_index + 1) % max_queue_size;
    queue->item_count--;
    return true;
  } else
    return false;
}

size_t queue__get_item_count(const queue_s *queue) { return queue->item_count; }