#include "config.h"


#pragma once

typedef struct
{
    httpd_req_t req[];
    size_t count;
} wsclient_list_t;


void init_wifi(void);
