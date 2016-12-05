
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: HTTP Server for ESP8266
 */


#ifndef __HTTP_H__
#define __HTTP_H__

#include <http_utils.h>
#include <utils.h>

#define HTTP_VER          "HTTP/1.1"
#define HTTP_EOL          "\r\n"


//
// Functions
//

void clean_path(char* path);

void http_dispatch(http_rqst* rqst);


// Parse HTTP Request
void http_parse_header(http_rqst* rqst, char* buff, u16 len);
void http_parse_header_fields(http_rqst* rqst, char* buff, u16 len);


// Receive Callback
void http_rcv(void *arg, char *buff, unsigned short len);

// Disconnection callback
void http_dsc(void* arg);

// HTTP Server Connection Callback
void http_server(void *arg);


#endif   // __HTTP_H__


