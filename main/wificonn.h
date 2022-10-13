#include "config.h"
#include "stdint.h"
#include "esp_http_server.h"


#pragma once

typedef struct
{
    httpd_req_t* req[MAX_CONN];
    size_t count;
} wsclient_list_t;


extern wsclient_list_t wsclient_list;


void init_wifi(void);
void wsclient_boardcast(uint8_t* data, size_t len);