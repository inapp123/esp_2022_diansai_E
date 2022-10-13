#include "config.h"
#include "digitalinput.h"


void init_digital_input(void){

    unsigned long long gpio_mask = 0;

    for(int i = 0; i < DIGITAL_INPUT_PIN_COUNT; i++){
        gpio_mask |= (1ULL << DIGITAL_INPUT_PINS[i]);
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = gpio_mask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };

    gpio_config(&io_conf);

}

uint32_t get_digital_input(void){
    uint32_t result = 0;
    for(int i = 0; i < DIGITAL_INPUT_PIN_COUNT; i++){
        result |= (gpio_get_level(DIGITAL_INPUT_PINS[i]) << i);
    }
    return result;
}

char* format_digital_input(uint32_t input){
    static char result[32];
    for(int i = 0; i < DIGITAL_INPUT_PIN_COUNT; i++){
        result[i] = (input & (1 << i)) ? '1' : '0';
    }

    sprintf(result + DIGITAL_INPUT_PIN_COUNT, ":%d", input);
    
    return result;
}