#include "s_queue_handler.h"

void s_queue__init(queue_s *queue, void *static_memory_for_queue, size_t static_memory_size_in_bytes) {
  uint8_t *dummy_ptr = (uint8_t *)&static_memory_for_queue;

  for (int i = 0; i < static_memory_size_in_bytes; i++) {
    queue->static_memory_for_queue = dummy_ptr;
    dummy_ptr++;
  }

  queue->static_memory_size_in_bytes = static_memory_size_in_bytes;
  queue->item_count = 0;
  queue->head_index = 0;
  queue->tail_index = 0;
}

bool s_queue__push(queue_s *queue, uint8_t push_value) {
  if ((queue->item_count) < (queue->static_memory_size_in_bytes)) {
    queue->static_memory_for_queue[queue->head_index] = push_value;
    queue->head_index = (queue->head_index + 1) % (queue->static_memory_size_in_bytes);
    queue->item_count++;
    return true;
  } else
    return false;
}

bool s_queue__pop(queue_s *queue, uint8_t *pop_value_ptr) {
  if (queue->item_count > 0) {
    *pop_value_ptr = queue->static_memory_for_queue[queue->tail_index];
    queue->tail_index = (queue->tail_index + 1) % (queue->static_memory_size_in_bytes);
    queue->item_count--;
    return true;
  } else
    return false;
}

size_t s_queue__get_item_count(const queue_s *queue) { return queue->item_count; }