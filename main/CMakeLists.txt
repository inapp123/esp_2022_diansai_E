idf_component_register(SRC_DIRS "."
                    REQUIRES "ssd1306" "driver" "nvs_flash" "esp_http_server" "esp_websocket_client"
                    EMBED_FILES "index.html"
                    INCLUDE_DIRS ".")
