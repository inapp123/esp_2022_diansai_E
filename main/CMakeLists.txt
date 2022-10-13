idf_component_register(SRCS "captdns.c" "wificonn.c" "digitalinput.c" "diansai.c"
                    REQUIRES "ssd1306" "driver" "nvs_flash" "esp_http_server"
                    INCLUDE_DIRS ".")
