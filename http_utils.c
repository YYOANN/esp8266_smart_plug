
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Utility functions for HTTP Server
 */

#include <http_utils.h>
#include <http.h>


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
                printf("HTTP Server: Memory Allocation Failed, %s:%d\n",
                                __FILE__, __LINE__);
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


