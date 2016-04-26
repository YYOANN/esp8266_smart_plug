
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: HTTP Handlers functions
 */


#ifndef __HTTP_HANDLERS_H__
#define __HTTP_HANDLERS_H__

#include <http_utils.h>
#include <http.h>

// Handlers Prototype
typedef void (*http_handler_fnc)(http_rqst*);
// Handlers
void index_handler(http_rqst* rqst);
void http_error(http_rqst* rqst);
void http_page_not_found(http_rqst* rqst);



// Handlers Table Association
typedef struct {const char* const path; const http_handler_fnc fnc;} http_handler;
extern const http_handler const http_handlers[];



#endif // __HTTP_HANDLERS_H__
