#include "wificonn.h"
#include "string.h"
#include "nvs_flash.h"

#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "config.h"
#include "esp_http_server.h"
#include "captdns.h"


const static char *TAG = "wificonn";

wsclient_list_t wsclient_list = {};
httpd_handle_t server = NULL;


void wsclient_list_add(int fd){
    if(wsclient_list.count < MAX_CONN){
        wsclient_list.fd[wsclient_list.count++] = fd;
    }
    else{
        ESP_LOGE(TAG, "too many connections");
    }
}

void wsclient_list_remove(int fd){
    for(int i = 0; i < wsclient_list.count; i++){
        if(wsclient_list.fd[i] == fd){
            wsclient_list.fd[i] = wsclient_list.fd[--wsclient_list.count];
            return;
        }
    }
    ESP_LOGE(TAG, "connection not found?!");
}

void ws_async_send(void *arg)
{
    ws_async_t *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = resp_arg->data;
    ws_pkt.len = resp_arg->len;
    ws_pkt.type = resp_arg->type;

    ESP_ERROR_CHECK(httpd_ws_send_frame_async(hd, fd, &ws_pkt));
    free(resp_arg);
}

void wsclient_boardcast(uint8_t* data, size_t len,httpd_ws_type_t type){
    for(int i = 0; i < wsclient_list.count; i++){
        int fd = wsclient_list.fd[i];
        ws_async_t *resp_arg = malloc(sizeof(ws_async_t));
        resp_arg->hd = server;
        resp_arg->fd = fd;
        resp_arg->data = data;
        resp_arg->len = len;
        resp_arg->type = type;
        ESP_ERROR_CHECK(httpd_queue_work(server, ws_async_send, resp_arg));
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}


extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");

esp_err_t index_handler(httpd_req_t *req)
{
    /* Send a simple response */
    httpd_resp_send(req, index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t captive_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "http://192.168.4.1/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

esp_err_t ws_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "ws_handler: method - %d", req->method);
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        ESP_LOGI(TAG,"req:0x%p",req);
        wsclient_list_add(httpd_req_to_sockfd(req));
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);

    if(ws_pkt.type == HTTPD_WS_TYPE_CLOSE){
        ESP_LOGI(TAG, "Connection closed");
        wsclient_list_remove(httpd_req_to_sockfd(req));
    }
    else{
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
    }
    
    free(buf);
    return ret;
}

static const httpd_uri_t ws_uri = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true
};

static const httpd_uri_t index_uri = {
        .uri        = "/",
        .method     = HTTP_GET,
        .handler    = index_handler,
        .user_ctx   = NULL,
        .is_websocket = false
};

static const httpd_uri_t captive_uri = {
        .uri        = "/generate_204",
        .method     = HTTP_GET,
        .handler    = captive_handler,
        .user_ctx   = NULL,
        .is_websocket = false
};



httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &captive_uri);
        httpd_register_uri_handler(server, &ws_uri);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


void init_wifi(void){
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 0,
            .password = WIFI_PWD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // get esp ip address
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    ESP_LOGI(TAG,"IP address is: %d.%d.%d.%d",IP2STR(&ip_info.ip));
    start_webserver();

    captdnsInit();

    esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), ESP_NETIF_DNS_MAIN, &ip_info.ip);
    esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), ESP_NETIF_DNS_BACKUP, &ip_info.ip);

}