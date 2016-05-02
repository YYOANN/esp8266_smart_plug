
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: HTTP Handlers functions
 */



#include <user_config.h>
#include <utils.h>
#include <http_utils.h>
#include <http_handlers.h>
#include <http.h>


const http_handler const http_handlers[] =
{
        {"/",           index_handler       },
        {"/index.html", index_handler       },
        {"/index.php" , index_handler       },
        {0,             0                   },
};


void index_handler(http_rqst* rqst)
{
        printf("HTTP Server: \"/\" Handler\n");

        static unsigned char sw = 0;    // Switch

        if(!rqst) return;
        if(!rqst->connection) return;

        http_rsp* rsp = (http_rsp*)zalloc(sizeof(http_rsp));
        rqst->rsp = rsp;

        rsp->connection = rqst->connection;
        rsp->code = get_rsp_code(200);

        const char on_body[] = "<html><body><h1>SW ON</h1></body></html>";
        const char off_body[] = "<html><body><h1>SW OFF</h1></body></html>";

        char content_length[16];
        if(sw)
        {
                printf("HTTP Server: Sitch ON\n");
                GPIO_SET_HIGH(GPIO_14);
                sw = 0;
                rsp->body_length = strlen(on_body);
                rsp->body = (char*)malloc(rsp->body_length);
                memcpy(rsp->body, on_body, rsp->body_length);
                snprintf(content_length, 16, "%d", rsp->body_length);
                http_add_rsp_header(rsp, "Content-Length", content_length);
                send_response(rsp);
        }
        else
        {
                printf("HTTP Server: Sitch OFF\n");
                GPIO_SET_LOW(GPIO_14);
                sw = 1;
                rsp->body_length = strlen(off_body);
                rsp->body = malloc(rsp->body_length);
                memcpy(rsp->body, off_body, rsp->body_length);
                snprintf(content_length, 16, "%d", rsp->body_length);
                http_add_rsp_header(rsp, "Content-Length", content_length);
                send_response(rsp);
        }
}


void http_error(http_rqst* rqst)
{
        if(!rqst) return;
        if(!rqst->connection) return;
        char content_length[16];
        const char msg500[] = "<html><body>"
                              "<h1>Server Error :(</h1>"
                              "</body></html>";
        http_rsp* rsp = (http_rsp*)zalloc(sizeof(http_rsp));
        rqst->rsp = rsp;
        rsp->connection = rqst->connection;
        rsp->code = get_rsp_code(500);
        rsp->body_length = strlen(msg500);
        rsp->body = malloc(rsp->body_length);
        memcpy(rsp->body, msg500, rsp->body_length);
        snprintf(content_length, 16, "%d", rsp->body_length);
        http_add_rsp_header(rsp, "Content-Length", content_length);
        send_response(rsp);
}

void http_page_not_found(http_rqst* rqst)
{
        if(!rqst) return;
        if(!rqst->path)
                printf("Page Not Found, no path info\n");
        else
                printf("Page \"%s\" Not Found\n", rqst->path);

        if(!rqst->connection) return;
        printf("404 Response Connection OK... \n");

        char content_length[16];
        const char msg404[] = "<html><body>"
                              "<h1>This page does not exist :(</h1>"
                              "</body></html>";

        http_rsp* rsp = (http_rsp*)zalloc(sizeof(http_rsp));
        rqst->rsp = rsp;
        rsp->connection = rqst->connection;

        rsp->code = get_rsp_code(404);
        printf("Code is 0x%p\n", rsp->code);

        rsp->body_length = strlen(msg404);
        rsp->body = malloc(rsp->body_length);
        memcpy(rsp->body, msg404, rsp->body_length);

        snprintf(content_length, 16, "%d", rsp->body_length);
        http_add_rsp_header(rsp, "Content-Length", content_length);

        printf("Send 404 Response ... \n");
        send_response(rsp);
}
