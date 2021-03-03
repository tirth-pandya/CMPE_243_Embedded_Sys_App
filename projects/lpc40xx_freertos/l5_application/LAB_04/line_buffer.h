#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  void *memory;
  size_t max_size;
  size_t write_index;
} line_buffer_s;

void line_buffer__init(line_buffer_s *buffer, void *memory, size_t size);

bool line_buffer__add_byte(line_buffer_s *buffer, char byte);

bool line_buffer__remove_line(line_buffer_s *buffer, char *line, size_t line_max_size);
