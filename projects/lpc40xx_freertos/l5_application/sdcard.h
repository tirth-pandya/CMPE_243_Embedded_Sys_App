#pragma once

#include "ff.h"
#include "stdbool.h"
#include "stdint.h"

FRESULT file__read(const char *filename, void *data_to_read, uint32_t byte_to_read, UINT *bytes_read);

unsigned long file__get_size(const char *filename);

bool file__is_eof(const char *filename);

void file__reset_offset(void);