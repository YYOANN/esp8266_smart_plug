
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Main file for Smart Plug App
 *       on ESP8266
 */

#include <user_config.h>
#include <utils.h>
#include <driver/uart.h>
#include <http.h>
#include <espconn.h>

#define WEB_PORT        80

// Wifi Info
const char wifi_ssid[32] = "my_ap_ssid";
const char wifi_pwd[32] = "my_ap_password";

void user_init()
{
        // Init UART
        {
                // Try BIT_RATE_460800 and BIT_RATE_921600
                uart_init(BIT_RATE_115200, BIT_RATE_115200);
                printf("Boot...\n");
        }

        // Init OI Pins
        {
                printf("Init IO\n");
                // Set GPIO 14 and 16 to OUTPUT
                PIN_FUNC_SELECT(GPIO_12_PIN, FUNC_GPIO12);
                PIN_FUNC_SELECT(GPIO_14_PIN, FUNC_GPIO14);
                GPIO_SET_MODE_OUTPUT(GPIO_14);
                GPIO_SET_MODE_OUTPUT(GPIO_12);
        }

        // Connect to Wifi Network
        {
                printf("Connect WIFI\n");
                struct station_config sc;
                memcpy(&sc.ssid, wifi_ssid, 32);
                memcpy(&sc.password, wifi_pwd, 32);

                wifi_set_opmode( STATION_MODE );
                wifi_station_set_config(&sc);
                wifi_station_connect();
        }

        // Create an TCP listner for port 80
        {
                printf("Create TCP Listner on port %d\n", WEB_PORT);
                struct espconn* c =
                        (struct espconn *)zalloc(sizeof(struct espconn));

                espconn_create(c);
                c->type = ESPCONN_TCP;
                c->state = ESPCONN_NONE;

                c->proto.tcp = (esp_tcp *)malloc(sizeof(esp_tcp));
                c->proto.tcp->local_port = WEB_PORT;

                // Set New Connection Callback
                espconn_regist_connectcb(c, http_server);

                // Enable TCP listner
                espconn_accept(c);
                espconn_regist_time(c, 15, 0);
        }

}
