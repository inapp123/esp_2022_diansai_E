#include <stdio.h>
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include "wificonn.h"
#include "string.h"


#include "digitalinput.h"

const static char *TAG = "main";


void app_main(void)
{


    SSD1306_t dev;

	i2c_master_init(&dev, I2C_SDA, I2C_SCL, -1);
    ssd1306_init(&dev, 128, 32);

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);


#ifdef WORK_AS_AP
    ssd1306_display_text(&dev, 0, "AP Mode, TX off", 16, false);
#else
    ssd1306_display_text(&dev, 0, "STA Mode, TX off", 17, false);
#endif
    

    init_digital_input();
    init_wifi();


    while(1){
        // ESP_LOGI(TAG,"hello world! digital input is: %d", get_digital_input());
        ssd1306_clear_line(&dev, 1,false);

        uint32_t digitalinputs = get_digital_input();
        char* formated_str = format_digital_input(digitalinputs);
        ssd1306_display_text(&dev, 1, formated_str, strlen(formated_str), false);

        uint8_t data[4];
        *(uint32_t*)data = digitalinputs;
        wsclient_boardcast(data,4);
        vTaskDelay(pdMS_TO_TICKS(200));

    }
}
