#include "unity.h"

#include "Mockmessage.h"

#include "message_processor.h"

// This only tests if we process at most 3 messages
void test_process_3_messages(void) {
  message_s test_message_1 = {.data = "abcd\0"};
  message_s test_message_2 = {.data = "bcde\0"};
  message_s test_message_3 = {.data = "cdef\0"};

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_1);

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_2);

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_3);

  // Since we did not return a message that starts with '$' this should return false
  TEST_ASSERT_FALSE(message_processor());
}

void test_process_3_messages_alternate(void) {
  message_s test_message_1 = {.data = "bacd\0"};
  message_s test_message_2 = {.data = "bbcd\0"};
  message_s test_message_3 = {.data = "#bcd\0"};

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_1);

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_2);

  message__read_ExpectAndReturn(NULL, true);
  message__read_IgnoreArg_message_to_read();
  message__read_ReturnThruPtr_message_to_read(&test_message_3);

  // Since we did not return a message that starts with '$' this should return false
  TEST_ASSERT_TRUE(message_processor());
}
void test_process_message_with_dollar_sign(void) {
  message_s test_message = {.data = "$abc\0"};
  TEST_ASSERT_TRUE(check_first_byte(test_message));
}

void test_process_messages_without_any_dollar_sign(void) {
  message_s test_message = {.data = "xyz\0"};
  TEST_ASSERT_FALSE(check_first_byte(test_message));
}

void test_process_message_with_pound_sign(void) {
  message_s test_message = {.data = "#abc\0"};
  TEST_ASSERT_TRUE(check_first_byte(test_message));
}

void test_process_messages_without_any_pound_sign(void) {
  message_s test_message = {.data = "xyz\0"};
  TEST_ASSERT_FALSE(check_first_byte(test_message));
}

// Add more tests if necessary