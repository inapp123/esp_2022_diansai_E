#include "analoginput.h"
#include "config.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "wificonn.h"
#include "esp_http_server.h"

//----------------------------------------------------------------------------------------//
// I2S setup for ADC sampling
//----------------------------------------------------------------------------------------//
void init_i2s()
{
	// configure I2S
	i2s_config_t i2s_config;
	i2s_config.mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN;
	i2s_config.sample_rate = I2S_SAMPLE_RATE;
	i2s_config.dma_buf_len = ADC_SAMPLES_COUNT;
	i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
	// i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
	i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
	i2s_config.use_apll = false,
	i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
	i2s_config.intr_alloc_flags = 0;
	i2s_config.dma_buf_count = 5;

	// install and start i2s driver
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_CHANNEL_7, ADC_ATTEN_11db));
	ESP_ERROR_CHECK( i2s_driver_install(I2S_NUM_0, &i2s_config,  0, NULL) );
	ESP_ERROR_CHECK( i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_7) );

	//vTaskDelay(5000 / portTICK_RATE_MS);
	//printf("Done waiting, enable ADC... \n");

	ESP_ERROR_CHECK( i2s_adc_enable(I2S_NUM_0) );
}


extern int do_tx;
void analoginputTask(void* pvParameter){
    init_i2s();
    uint8_t* buf = malloc(ADC_SAMPLES_COUNT * sizeof(uint16_t) + 1);
    while(true){
        size_t bytes_read;
        ESP_ERROR_CHECK( i2s_read(I2S_NUM_0, buf + 1, ADC_SAMPLES_COUNT * sizeof(uint16_t), &bytes_read, portMAX_DELAY) );
        buf[0] = WS_TYPE_ANALOG;

        if(do_tx){
            wsclient_boardcast(buf,ADC_SAMPLES_COUNT * sizeof(uint16_t) + 1, HTTPD_WS_TYPE_BINARY);
        }
    }

}




