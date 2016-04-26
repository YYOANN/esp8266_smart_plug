
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: HTTP Handlers functions
 */



#include<user_config.h>
#include<utils.h>
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

        struct espconn* c = rqst->connection;


        // Register Sent Callback
        espconn_regist_sentcb(c, http_dsc);

        if(sw)
        {
                printf("HTTP Server: Sitch ON\n");
                GPIO_SET_HIGH(GPIO_14);
                sw = 0;
                espconn_sent(c, (uint8*)"HTTP/1.1 200 OK\n\r\n\r"
                                        "<html><body><h1>SW ON      "
                                        "</h1></body></html>\n\r", 66);
        }
        else
        {
                printf("HTTP Server: Sitch OFF\n");
                GPIO_SET_LOW(GPIO_14);
                sw = 1;
                espconn_sent(c, (uint8*)"HTTP/1.1 200 OK\n\r\n\r"
                                        "<html><body><h1>SW OFF     "
                                        "</h1></body></html>\n\r", 66);
        }
}


// TODO: Implement 404 et 500
void http_error(http_rqst* rqst)
{
        if(!rqst) return;
        if(!rqst->connection) return;
        espconn_disconnect(rqst->connection);
}

void http_page_not_found(http_rqst* rqst)
{
        if(!rqst) return;
        if(!rqst->path)
        {
                printf("Page Not Found, no path info\n");
        }
        else
        {
                printf("Page \"%s\" Not Found\n", rqst->path);
        }

        if(rqst->connection)
                espconn_disconnect(rqst->connection);
}
