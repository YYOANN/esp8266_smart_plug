
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: HTTP Server for ESP8266
 */



#include <user_config.h>
#include <xtmissing.h>
#include <utils.h>
#include <http_handlers.h>
#include <http_utils.h>
#include <http.h>



// Dispatch HTTP Request to hander
void http_dispatch(http_rqst* rqst)
{
        printf("HTTP Server: Dispatch\n");

        u16 i = 0;
        if(!rqst) return;
        if(!rqst->path)
        {
                http_error(rqst);
        }


        while(http_handlers[i].path)
        {
                if(!strcmp(http_handlers[i].path, rqst->path))
                {
                        http_handlers[i].fnc(rqst);
                        break;
                }
                i++;
        }

        // Page Not found
        if(!http_handlers[i].path)
        {
                http_page_not_found(rqst);
        }
}

// Parse HTTP Header Fields, return Next line in http buffer, NULL if error
// Fields format:  NAME: VALUE\n\r
void http_parse_header_fields(http_rqst* rqst, char* buff, u16 len)
{
        if(!rqst || !buff || !len)
                return;


        char* buff_end = buff + len;
        char* curr = buff;
        char* body = NULL;
        u16 count = 0;


        // Add headers fields
        while(count < MAX_HEADERS)
        {
                rqst->headers[count].name = curr;
                curr = search_space(curr, buff_end-curr);
                if(!curr)
                {
                        rqst->headers[count].name = NULL;
                        break;
                }

                // Terminate Name (replace ':' with '\0')
                *(curr-1) = '\0';

                rqst->headers[count].value = ++curr;
                curr = search_http_eol(curr, buff_end-curr);
                if(!curr)
                {
                        rqst->headers[count].name = NULL;
                        rqst->headers[count].value = NULL;
                        break;
                }

                // Terminate Value (replace '\n' with '\0')
                *curr = '\0';
                curr += HTTP_EOL_LEN;


                // Check if end of header fields
                body = search_http_eol(curr, buff_end-curr);

                if(body == curr)
                {
                        body += HTTP_EOL_LEN;
                        break;
                }
                else
                {
                        body = NULL;
                }

                count++;
        }

        // Save body start and length
        if(body >= buff_end || !body)
        {
                rqst->body = NULL;
                rqst->body_length = 0;
        }
        else
        {
                rqst->body = body;
                rqst->body_length = buff_end-body;
        }
}

// Parse HTTP Header
void http_parse_header(http_rqst* rqst, char* buff, u16 len)
{
        printf("HTTP Server: Parsing Header\n");
        if(!rqst || !buff || !len)
                return;

        // Parse fist line: METHOD  PATH  HTTP_VER
        {
                char* curr;      // Current position
                char* buff_end = buff+len;
                rqst->method = check_http_method(buff, len);
                if(!rqst->method) return;

                curr = search_space(buff, len);
                if(!curr) return;

                rqst->path = ++curr;

                curr = search_space(curr, buff_end-curr);
                if(!curr)
                {
                        rqst->path = NULL;
                        return;
                }

                // Terminate PATH
                *curr++ = '\0';


                if(check_valid_http(curr, buff_end-curr)) return;

                // OK Increment pointer to start of next line
                curr = search_http_eol(curr, buff_end-curr);
                if(!curr) return;


                curr += HTTP_EOL_LEN;

                if(curr < buff_end)
                        http_parse_header_fields(rqst, curr, buff_end-curr);
        }
}


// Receive Callback
void ICACHE_FLASH_ATTR http_rcv(void *arg, char *buff, unsigned short len)
{
        struct espconn *c = (struct espconn *)arg;

        *(buff+len-1) = '\0';

        printf("HTTP Server: Receive Data length: %d\n", len);

        if(!c) return;

        http_rqst* rqst = (http_rqst*) zalloc(sizeof(http_rqst));
        rqst->connection = c;
        c->reverse = rqst;

        http_parse_header(rqst, buff, len);

        if(rqst->path)
        {
                http_dispatch(rqst);
        }
        else
        {
                http_error(rqst);
        }

}


// Disconnection callback
void http_dsc(void* arg)
{
        struct espconn *c = (struct espconn *)arg;

        if(!c) return;

        printf("HTTP Server: Disconnecting\n");

        short i = 0;
        http_rsp* rsp;
        http_rqst* rqst;

        if(c->reverse)
        {
                rqst = (http_rqst*)c->reverse;
                rsp = rqst->rsp;
                if(rsp)
                {
                        while(i < MAX_HEADERS)
                        {
                                if(rsp->headers[i].name)
                                        free(rsp->headers[i].name);
                                rsp->headers[i].name = NULL;

                                if(rsp->headers[i].value)
                                        free(rsp->headers[i].value);
                                rsp->headers[i].value = NULL;

                                i++;
                        }

                        i = 0;
                        while(i < HTTP_RSP_BUF_LEN)
                        {
                                if(rsp->buffers[i].allocated)
                                {
                                        if(rsp->buffers[i].buff)
                                                free(rsp->buffers[i].buff);
                                }

                                i++;
                        }

                        if(rsp->body)
                                free(rsp->body);
                        rsp->body = NULL;

                        free(rsp);
                        rsp = NULL;
                }

                free(c->reverse);
                c->reverse = NULL;
        }
}


// HTTP Server Connection Callback
void ICACHE_FLASH_ATTR http_server(void *arg)
{
        struct espconn *c = (struct espconn *)arg;

        printf("HTTP Server: Server Callback\n");

        // Receive Callback
        espconn_regist_recvcb(c, http_rcv);

        // Disconnect Callback
        espconn_regist_disconcb(c, http_dsc);
}

