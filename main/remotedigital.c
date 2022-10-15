#include "remotedigital.h"
#include "config.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "wificonn.h"
#include "esp_log.h"

const static char *TAG = "remote-di";
uint32_t remote_digital = 0;
esp_websocket_client_handle_t client;
int remote_connected = false;


void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        remote_connected = true;
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        remote_connected = false;
        break;
    case WEBSOCKET_EVENT_DATA:
        if(data->data_len == 2 && data->data_ptr[0] == WS_TYPE_DIGITAL){
            remote_digital = data->data_ptr[1];
            ESP_LOGI(TAG, "OKOK,this is correct, %d",remote_digital);
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}


void init_remote_digital(void){
    const esp_websocket_client_config_t ws_cfg = {
#ifdef WORK_AS_AP
        .uri = "ws://192.168.4.250/ws_digital",
#else
        .uri = "ws://192.168.4.1/ws_digital",
#endif
        
    };
    client = esp_websocket_client_init(&ws_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    esp_websocket_client_start(client);
}