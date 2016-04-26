
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Utility functions for HTTP Server
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <espconn.h>

// Length of a string defined at compile time
#define CONST_STRLEN(s)         (sizeof(s)-1)


#define GPIO_1          1
#define GPIO_12         12
#define GPIO_14         14

#define GPIO_12_PIN     (PERIPHS_IO_MUX_MTDI_U)
#define GPIO_14_PIN     (PERIPHS_IO_MUX_MTMS_U)


// GPIO
#define GPIO_SET_MODE_OUTPUT(GPIO)      (gpio_output_set(0, 0, 1 << GPIO, 0))
#define GPIO_SET_MODE_INPUT(GPIO)       (gpio_output_set(0, 0, 0, 1 << GPIO))
#define GPIO_SET_HIGH(GPIO)             (gpio_output_set(1 << GPIO, 0, 0, 0))
#define GPIO_SET_LOW(GPIO)              (gpio_output_set(0, 1 << GPIO, 0, 0))




// Redefine int
typedef unsigned char u8;
typedef short i8;
typedef unsigned short u16;
typedef short i16;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long long u64;
typedef long long i64;

#endif // __UTILS_H__
