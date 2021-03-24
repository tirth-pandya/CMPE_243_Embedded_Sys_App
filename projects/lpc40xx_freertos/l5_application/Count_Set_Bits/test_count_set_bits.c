#include "unity.h"

#include <stdint.h>

#include "count_set_bits.c"

void setUp(void) {}

void tearDown(void) {}

void test_count_set_bits(void) {
  uint32_t test_val = 0x01;
  uint32_t expected_result = 1;
  uint32_t returned_result = count_bits(test_val);
  TEST_ASSERT_EQUAL_UINT32(returned_result, expected_result);

  test_val = 0xffffffff;
  expected_result = 32;
  returned_result = count_bits(test_val);
  TEST_ASSERT_EQUAL_UINT32(returned_result, expected_result);

  test_val = 0xffff;
  expected_result = 16;
  returned_result = count_bits(test_val);
  TEST_ASSERT_EQUAL_UINT32(returned_result, expected_result);

  test_val = 0xffff;
  expected_result = 16;
  returned_result = count_bits(test_val);
  TEST_ASSERT_EQUAL_UINT32(returned_result, expected_result);

  test_val = 0;
  expected_result = 0;
  returned_result = count_bits(test_val);
  TEST_ASSERT_EQUAL_UINT32(returned_result, expected_result);
}