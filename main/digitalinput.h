#pragma once
#include "driver/gpio.h"

void init_digital_input(void);
uint32_t get_digital_input(void);
char* format_digital_input(uint32_t input);