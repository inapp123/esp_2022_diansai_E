#pragma once

#define I2C_SDA 32
#define I2C_SCL 33

#define CONFIG_OFFSETX 0

// work as ap, if not defined, work as station
// #define WORK_AS_AP 1

#define WIFI_SSID      "SRGNTMG"
#define WIFI_PWD       "SRGNTMGDIANSAI"


const static int DIGITAL_INPUT_PINS[] = {23,22,19,18,5,4,21,15}; // msb to lsb
#define DIGITAL_INPUT_PIN_COUNT 8

#define MAX_CONN 10

#define ADC_SAMPLES_COUNT 512
#define I2S_SAMPLE_RATE 44100
