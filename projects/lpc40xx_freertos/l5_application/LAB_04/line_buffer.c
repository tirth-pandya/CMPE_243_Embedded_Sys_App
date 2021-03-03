#include "line_buffer.h"

#include <stdlib.h>
#include <string.h>

void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size) {
  if (buffer != NULL) {
    buffer->memory = memory;
    buffer->max_size = size;
    buffer->write_index = 0;
  }
}

bool line_buffer__add_byte(line_buffer_s *buffer, char byte) {
  bool add_byte_status = false;

  if (buffer->write_index < buffer->max_size) {
    memcpy((buffer->memory) + (buffer->write_index), &byte, sizeof(char));
    buffer->write_index++;
    return add_byte_status = true;
  } else
    return add_byte_status = false;
}

bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size) {
  bool line_remove_status = false;

  if (buffer == NULL || line == NULL) {
    return line_remove_status = false;
  }

  else {
    static const char *line_delimiter = "\n";
    char *memory_index = buffer->memory;
    char dummy_line[128] = {0};
    char *temp_char;

    memcpy(dummy_line, memory_index, buffer->write_index); // Copied the buffer data to dummy line

    // Check the dummy line for line delimiter
    temp_char = strstr(dummy_line, line_delimiter);
    if (temp_char != NULL) {

      // Check if valid data available before delimiter
      char *string_token = strtok(dummy_line, line_delimiter);
      if (string_token != NULL) {
        memcpy(line, string_token, line_max_size - sizeof(char));
        memmove(memory_index, (memory_index + strlen(string_token) + sizeof(char)), buffer->max_size);
        buffer->write_index -= (strlen(string_token) + sizeof(char));
        line_remove_status = true;
      }
    }

    else if (buffer->write_index >= line_max_size) {
      memcpy(line, dummy_line, line_max_size - sizeof(char));
      memmove(memory_index, (memory_index + strlen(line)), buffer->max_size);
      buffer->write_index = buffer->write_index - (strlen(line) + sizeof(char));
      line_remove_status = true;
    }

    else {
      line_remove_status = false;
    }
  }
  return line_remove_status;
}