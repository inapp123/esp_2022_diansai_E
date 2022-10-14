idf_component_register(SRC_DIRS "."
                    REQUIRES "ssd1306" "driver" "nvs_flash" "esp_http_server"
                    EMBED_FILES "index.html"
                    INCLUDE_DIRS ".")
