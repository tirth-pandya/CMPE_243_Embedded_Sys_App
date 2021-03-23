#include "sdcard.h"

static uint32_t song_offset = 0;

FRESULT file__read(const char *filename, void *data_to_read, uint32_t byte_to_read, UINT *bytes_read) {
  FRESULT result;
  FIL file;

  result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
  if (FR_OK == result) {
    if (song_offset) {
      f_lseek(&file, song_offset);
    }
    result = f_read(&file, data_to_read, byte_to_read, bytes_read);
    song_offset += byte_to_read;
    f_close(&file);
  }

  return result;
}

unsigned long file__get_size(const char *filename) {
  FIL file;
  FRESULT result = f_open(&file, filename, FA_OPEN_ALWAYS | FA_READ);
  unsigned long file_size = 0;
  if (FR_OK == result) {
    file_size = f_size(&file);
    f_close(&file);
  }
  return file_size;
}

bool file__is_eof(const char *filename) {
  FIL file;
  FRESULT result = f_open(&file, filename, FA_OPEN_ALWAYS | FA_READ);
  bool status = true;
  if (FR_OK == result) {
    if (song_offset) {
      f_lseek(&file, song_offset);
    }
    if (!f_eof(&file)) {
      status = false;
    } else {
      song_offset = 0;
    }
    f_close(&file);
  }
  return status;
}

void file__reset_offset(void) { song_offset = 0; }
