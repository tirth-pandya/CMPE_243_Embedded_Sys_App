#include "unity.h"

#include <stddef.h>
#include <stdint.h>

#include "get_highest.c"

const uint32_t test_array[5] = {5, 10, 4, 1, 2};
void setUp(void) {}

void tearDown(void) {}

void test_array_size_invalid_return_zero(void) {
  uint32_t expected_retVal = 0;
  uint32_t actual_retVal;

  actual_retVal = get_highest(test_array, 0);
  TEST_ASSERT_EQUAL_UINT32(expected_retVal, actual_retVal);
}

void test_array_pointing_to_null_return_zero(void) {
  uint32_t expected_retVal = 0;
  uint32_t actual_retVal;

  actual_retVal = get_highest(NULL, 5);
  TEST_ASSERT_EQUAL_UINT32(expected_retVal, actual_retVal);
}

void test_array_correct_highest_value(void) {
  uint32_t expected_retVal = 10;
  uint32_t actual_retVal;

  actual_retVal = get_highest(test_array, 5);
  TEST_ASSERT_EQUAL_UINT32(expected_retVal, actual_retVal);
}