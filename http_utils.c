
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Utility functions for HTTP Server
 */

#include <http_utils.h>
#include <http.h>
#include <utils.h>


// HTTP Status Code {code, txt}
const rsp_code const rsp_codes[] =
{
        {100, "100 Continue"         },
        {200, "200 OK"               },
        {201, "201 Created"          },
        {301, "301 Moved Permanently"},
        {400, "400 Bad Request"      },
        {401, "401 Unauthorized"     },
        {403, "403 Forbidden"        },
        {404, "404 Not Found"        },
        {0,   0                  },
};


// HTTP Request Type {code, txt, len}
const rqst_method const rqst_methods[] =
{
        {POST,   "POST",    CONST_STRLEN("POST")    },
        {GET,    "GET",     CONST_STRLEN("GET")     },
        {PUT,    "PUT",     CONST_STRLEN("PUT")     },
        {PATCH,  "PATCH",   CONST_STRLEN("PATCH")   },
        {DELETE, "DELETE",  CONST_STRLEN("DELETE")  },
        {0,      0,         0                       },
};

// Clean request path
// Remove multiple '/' and trailing '/'
void clean_path(char* path)
{
        short i = 0;
        short j = 0;
        char curr;
        char prev = '\0';
        char* tmp;
        if(!path) return;


        tmp = (char*) malloc(strlen(path)+1);
        if(!tmp)
        {
                ALLOC_ERROR();
        }

        printf("HTTP Server: Clean Path %s\n", path);

        if(path && tmp)
        {

                // Remove multiple '/', consider ' ' as end of path
                curr = path[i];
                while(curr && curr != ' ')
                {
                        if(!(prev == '/' && curr == '/'))
                        {
                                tmp[j++] = path[i];
                        }

                        prev = curr;
                        curr = path[++i];
                }

                // Remove trailing '/'
                if(j > 1)
                {
                        while(tmp[--j] == '/' && j > 1){};

                        tmp[j+1] = '\0';
                        strcpy(path, tmp);
                }
        }

        if(tmp) free(tmp);
}


// Check if HTTP Method is ok, return pointer to method or NULL
const rqst_method* check_http_method(char* buff, uint16 len)
{

        u16 i = 0;
        while(rqst_methods[i].code)
        {
                if(rqst_methods[i].len < len)
                {
                        if(!strncmp(buff, rqst_methods[i].txt,
                                                rqst_methods[i].len))
                        {
                                return &rqst_methods[i];
                        }
                }
                i++;
        }

        return NULL;
}

// Check if HTTP Version is ok, return 0 if ok
i32 check_valid_http(char* buff, uint16 len)
{

        if(len >= strlen(HTTP_VER))
        {
                return strncmp(buff, HTTP_VER, HTTP_VER_LEN);
        }

        return -1;
}

// Retrun next HTTP_EOL, NULL if not found
char* search_http_eol(char* buff, u16 len)
{
        char* limit = buff+len-1;
        while(buff < (limit - HTTP_EOL_LEN))
        {
                if(!strncmp(buff, HTTP_EOL, HTTP_EOL_LEN))
                        return buff;
                buff++;
        }
        return NULL;
}

// Retrun next space, NULL if not found
char* search_space(char* buff, u16 len)
{
        char* limit = buff+len;
        while(buff < limit)
        {
                if(*(buff++) == ' ')
                        return --buff;
        }
        return NULL;
}



// Generate HTTP response buffer from http_rsp
char* get_rsp(http_rsp* rsp)
{
        if(!rsp) return NULL;
        if(!rsp->code) return NULL;

        char* buff = NULL;
        // Compute final size
        u16 i = 0;
        u32 len = HTTP_VER_LEN + strlen(rsp->code->txt) + HTTP_EOL_LEN + 2;

        while(rsp->headers[i].name)
        {
                if(rsp->headers[i].value)
                {
                        len += strlen(rsp->headers[i].name)
                                + strlen(rsp->headers[i].value)
                                + HTTP_EOL_LEN
                                + 2; // ": "
                }
                i++;
        }

        len += HTTP_EOL_LEN + rsp->body_length;

        // Allocate memory
        buff = (char*) malloc(len);
        if(!buff)
        {
                ALLOC_ERROR();
                return NULL;
        }

        // Copy info
        char* curr = buff;
        const char* src = HTTP_VER;

        while(*src){ *curr++ = *src++;}
        *curr++ = ' ';
        src = rsp->code->txt;
        while(*src){ *curr++ = *src++;}

        memcpy(curr, HTTP_EOL, HTTP_EOL_LEN);
        curr += HTTP_EOL_LEN;

        i = 0;
        while(rsp->headers[i].name)
        {
                src = rsp->headers[i].name;
                while(*src)
                {
                        *curr++ = *src++;
                }

                *curr++ = ':';
                *curr++ = ' ';

                src = rsp->headers[i].value;
                while(*src)
                {
                        *curr++ = *src++;
                }

                memcpy(curr, HTTP_EOL, HTTP_EOL_LEN);
                curr += HTTP_EOL_LEN;

                i++;
        }

        memcpy(curr, HTTP_EOL, HTTP_EOL_LEN);
        curr += HTTP_EOL_LEN;

        if(rsp->body && rsp->body_length)
        {
                memcpy(curr, rsp->body, rsp->body_length);
        }

        return buff;
}



const rsp_code* get_rsp_code(short code)
{
        short i = 0;
        while(rsp_codes[i].code)
        {
                if(rsp_codes[i].code == code)
                        return &rsp_codes[i];
                i++;
        }
        return NULL;
}

void disco_callback(void* arg)
{
        printf("Diso Callback/n");
        espconn_disconnect(arg);
}

void more_sent_callback(void* arg)
{
        printf("More Callback\n");
        struct espconn* c = (struct espconn*) arg;
        if(!c) return;
        http_rsp* rsp = ((http_rqst*) c->reverse)->rsp;
        if(!rsp) return;

        u16 curr = rsp->curr_buff + 1;
        if(curr >= HTTP_RSP_BUF_LEN) return;

        if(!rsp->buffers[curr].buff || !rsp->buffers[curr].len)
        {
                printf("Disconnect with curr = %d, buff=0x%p, len=%d!!!\n",
                                curr, rsp->buffers[curr].buff, rsp->buffers[curr].len);
                printf("RSP pointer: 0x%p\n", rsp);
                espconn_disconnect(arg);
        }
        else
        {
                ++rsp->curr_buff;
                if(rsp->reg_buff < curr+1)
                {
                        printf("Register Disco A\n");
                        espconn_regist_sentcb(c,
                                disco_callback);
                }
                else
                {
                        printf("Register More Callback\n");
                        espconn_regist_sentcb(c,
                                        more_sent_callback);
                }

                printf("Send Data B\n");
                espconn_sent(c, (u8*)rsp->buffers[curr].buff,
                                rsp->buffers[curr].len);
        }
}

void send_response(http_rsp* rsp)
{
        if(!rsp) return;
        if(!rsp->connection) return;
        if(!rsp->code) return;

        printf("HTTP Server: Sending Response.\n");

        struct espconn* c = rsp->connection;
        u16 i = 0;

        http_add_rsp_buffer(rsp, HTTP_VER, HTTP_VER_LEN, 0);
        http_add_rsp_buffer(rsp, " ", 1, 0);
        http_add_rsp_buffer(rsp, rsp->code->txt, strlen(rsp->code->txt), 0);
        http_add_rsp_buffer(rsp, HTTP_EOL, HTTP_EOL_LEN, 0);

        while(i < MAX_HEADERS)
        {
                if(!rsp->headers[i].name) break;
                if(!rsp->headers[i].value) break;

                http_add_rsp_buffer(rsp, rsp->headers[i].name,
                                strlen(rsp->headers[i].name), 0);

                http_add_rsp_buffer(rsp, ": ", 2, 0);

                http_add_rsp_buffer(rsp, rsp->headers[i].value,
                                strlen(rsp->headers[i].value), 0);

                http_add_rsp_buffer(rsp, HTTP_EOL, HTTP_EOL_LEN, 0);

                i++;
        }

        http_add_rsp_buffer(rsp, HTTP_EOL, HTTP_EOL_LEN, 0);
        http_add_rsp_buffer(rsp, rsp->body, rsp->body_length, 0);

        // Start sending first buffer
        espconn_regist_sentcb(c, more_sent_callback);
        espconn_sent(c, (u8*)rsp->buffers[0].buff, rsp->buffers[0].len);
}


void http_add_rsp_header(http_rsp* rsp, char* name, char* value)
{
        if(!rsp || !name || !value) return;

        u16 name_len = strlen(name);
        u16 val_len = strlen(value);

        if(!val_len || !name_len) return;
        name_len++;
        val_len++;

        u16 i = 0;
        while(i < MAX_HEADERS)
        {
                if(!rsp->headers[i].name && !rsp->headers[i].value)
                {
                        rsp->headers[i].name = (char*)malloc(name_len);
                        rsp->headers[i].value = (char*)malloc(val_len);
                        if(!rsp->headers[i].name || !rsp->headers[i].value)
                        {
                                ALLOC_ERROR();
                                break;
                        }
                        memcpy(rsp->headers[i].name, name, name_len);
                        memcpy(rsp->headers[i].value, value, val_len);
                        break;
                }
                i++;
        }
}



void http_add_rsp_buffer(http_rsp* rsp, const void* buff, u16 len, u16 copy)
{
        if(!rsp || !buff || !len) return;

        u16 i = 0;
        while(rsp->buffers[i].buff && i < HTTP_RSP_BUF_LEN) i++;

        if(i >= HTTP_RSP_BUF_LEN) return;  // No more space

        printf("Add to buffer %d: 0x%p len %d\n", i, buff, len);
        printf("RSP pointer: 0x%p\n", rsp);

        if(!copy)
        {
                rsp->buffers[i].buff = (char*)buff;
                rsp->buffers[i].len = len;
                rsp->buffers[i].allocated = 0;
                rsp->reg_buff++;
        }
        else
        {
                rsp->buffers[i].buff = malloc(len);
                if(!rsp->buffers[i].buff) return;
                memcpy(rsp->buffers[i].buff, buff, len);
                rsp->buffers[i].len = len;
                rsp->buffers[i].allocated = 1;
                rsp->reg_buff++;
        }
}

