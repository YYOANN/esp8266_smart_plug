
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Utility functions for HTTP Server
 */

#ifndef __HTTP_UTILS_H__
#define __HTTP_UTILS_H__


#include <user_config.h>
#include <utils.h>


#define MAX_HEADERS       (20)
#define HTTP_VER_LEN      (CONST_STRLEN(HTTP_VER))
#define HTTP_EOL_LEN      (CONST_STRLEN(HTTP_EOL))

enum http_req_type
{
        POST = 2,
        GET,
        PUT,
        PATCH,
        DELETE,
};

// HTTP Headers (Name: Value), alter buffer string:
// Add NULL into buffer to terminate name and value
typedef struct {char* name; char* value;} header;

// HTTP Request Type {code, txt, len}
typedef struct {const u16 code; const char* const txt; u16 len;} rqst_method;
extern const rqst_method const rqst_methods[];

// HTTP Status Code {code, txt}
typedef struct {const u16 code; const char* const txt;} rsp_code;
extern const rsp_code const rsp_codes[];


// HTTP Request Structure
// Holds info on a HTTP request
typedef struct
{
       const rqst_method*    method;
       header                headers[MAX_HEADERS];
       char*                 path;
       char*                 body;
       u32                   body_length;
       struct espconn*       connection;
} http_rqst;


// HTTP Response Structure
// Holds info on a HTTP response
typedef struct
{
       rsp_code*             code;
       header                headers[MAX_HEADERS];
       char*                 body;
       u32                   body_length;
       struct espconn*       connection;
} http_rsp;



// Clean request path
// Remove multiple '/' and trailing '/'
void clean_path(char* path);

// Check if HTTP Method is ok, return pointer to method or NULL
const rqst_method* check_http_method(char* buff, uint16 len);

// Check if HTTP Version is ok, return 0 if ok
i32 check_valid_http(char* buff, uint16 len);

// Retrun next HTTP_EOL, NULL if not found
char* search_http_eol(char* buff, u16 len);

// Retrun next space, NULL if not found
char* search_space(char* buff, u16 len);

char* get_rsp(http_rsp*);

#endif // __HTTP_UTILS_H__

