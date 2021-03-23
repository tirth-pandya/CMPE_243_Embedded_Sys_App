#pragma once

#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "acceleration.h"
#include "apds.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

QueueHandle_t orient_data;
QueueHandle_t gesture_data;

#define LED0 3
#define LED1 26
#define LED2 24
#define LED3 18

void orientation_task(void *p);
void perform_action_on_orientation(void *p);
