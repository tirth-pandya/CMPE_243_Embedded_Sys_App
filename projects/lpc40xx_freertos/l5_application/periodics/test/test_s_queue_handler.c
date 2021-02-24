#include "unity.h"

#include "s_queue_handler.h"

void test_comprehensive(void) {
  queue_s queue;
  const size_t max_allowed_size = 128;
  uint32_t test_dataset[128];

  s_queue__init(&queue, test_dataset, max_allowed_size);

  for (size_t item = 0; item < max_allowed_size; item++) {
    TEST_ASSERT_TRUE(s_queue__push(&queue, item));
    TEST_ASSERT_EQUAL(item + 1, s_queue__get_item_count(&queue));
  }

  // Should not be able to push anymore
  TEST_ASSERT_FALSE(s_queue__push(&queue, 100));
  TEST_ASSERT_EQUAL(max_allowed_size, s_queue__get_item_count(&queue));

  // Pull and verify the elements from Queue in FIFO order
  uint8_t popped_value = 0;
  for (size_t item = 0; item < max_allowed_size; item++) {
    TEST_ASSERT_TRUE(s_queue__pop(&queue, &popped_value));
    TEST_ASSERT_EQUAL(item, popped_value);
  }

  TEST_ASSERT_EQUAL(0, s_queue__get_item_count(&queue));
  TEST_ASSERT_FALSE(s_queue__pop(&queue, &popped_value));
}