#include <stdio.h>
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include "wificonn.h"
#include "string.h"

#include "digitalinput.h"
#include "analoginput.h"
#include "remotedigital.h"

const static char *TAG = "main";


SSD1306_t dev;

int do_tx = 0;
int64_t last_trigger_time = 0;
static void IRAM_ATTR gpio_isr_handler(void* arg){
    int64_t now_time = esp_timer_get_time();
    if(now_time - last_trigger_time > 300 * 1000){ // 300ms
        do_tx = !do_tx;
    }
    last_trigger_time = now_time;
}

extern wsclient_list_t wsclient_list;

extern int remote_connected;
uint32_t digitalinputs;
void screenUpdTask(void* pvParameter){
    i2c_master_init(&dev, I2C_SDA, I2C_SCL, -1);
    ssd1306_init(&dev, 128, 32);

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);


#ifdef WORK_AS_AP
    ssd1306_display_text(&dev, 0, "AP Mode, TX off", 16, false);
#else
    ssd1306_display_text(&dev, 0, "STA Mode, TX off", 17, false);
#endif

    static char buf[32];
    int buflen;

    while(1){
        ssd1306_clear_line(&dev, 0,false);
        if(do_tx){
#ifdef WORK_AS_AP
            ssd1306_display_text(&dev, 0, "AP Mode, TX on", 15, false);
#else
            ssd1306_display_text(&dev, 0, "STA Mode, TX on", 16, false);
#endif
        }
        else{
#ifdef WORK_AS_AP
            ssd1306_display_text(&dev, 0, "AP Mode, TX off", 16, false);
#else
            ssd1306_display_text(&dev, 0, "STA Mode, TX off", 17, false);
#endif
        }
        ssd1306_clear_line(&dev, 1,false);
        char* formated_str = format_digital_input(digitalinputs);
        ssd1306_display_text(&dev, 1, formated_str, strlen(formated_str), false);

        buflen = sprintf(buf, "ws:%d rdi:%s",wsclient_list.count,remote_connected ? "OK!" : "NG!");
        ssd1306_clear_line(&dev, 2,false);
        ssd1306_display_text(&dev, 2, buf, buflen, false);

        ssd1306_clear_line(&dev, 3,false);
        formated_str = format_digital_input(remote_digital);
        ssd1306_display_text(&dev, 3, formated_str, strlen(formated_str), false);

        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}


void app_main(void){

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << 0);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    // in some cases, the gpio isr service is already installed
    // so we needn't to ESP_ERROR_CHECK here
    gpio_install_isr_service(0);
    ESP_ERROR_CHECK(gpio_isr_handler_add(0, gpio_isr_handler, (void*) 0));


	
    

    init_digital_input();
    init_wifi();
    init_remote_digital();


    xTaskCreate(&screenUpdTask, "screenUpdTask", 2048, NULL, 2, NULL);
    xTaskCreate(&analoginputTask, "analoginputTask", 4096, NULL, 2, NULL);

    while(1){   
        // ESP_LOGI(TAG,"hello world! digital input is: %d", get_digital_input());

        digitalinputs = get_digital_input();

        uint8_t data[2];
        data[0] = WS_TYPE_DIGITAL;
        data[1] = digitalinputs;
        // if(do_tx){
            wsclient_boardcast(data,2,HTTPD_WS_TYPE_BINARY);
        // }
        wsclient_digital_boardcast(data,2,HTTPD_WS_TYPE_BINARY);
        vTaskDelay(pdMS_TO_TICKS(100));

    }
}
