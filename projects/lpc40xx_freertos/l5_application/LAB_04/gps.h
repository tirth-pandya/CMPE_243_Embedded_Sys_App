#pragma once

typedef struct {
  float longitude;
  float latitude;
} gps_coordinates_t;

void gps__run_once(void);

float gps__get_latitude(void);