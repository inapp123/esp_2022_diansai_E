#include "config.h"
#include "stdint.h"
#include "esp_http_server.h"


#pragma once

typedef struct
{
    int fd[MAX_CONN];
    size_t count;
} wsclient_list_t;

typedef struct {
    httpd_handle_t hd;
    int fd;
    uint8_t* data;
    size_t len;
    httpd_ws_type_t type;
} ws_async_t;

typedef enum {
    WS_TYPE_DIGITAL = 0,
    WS_TYPE_ANALOG,
} ws_type_t;


extern wsclient_list_t wsclient_list;


void init_wifi(void);
void wsclient_boardcast(uint8_t* data, size_t len,httpd_ws_type_t type);