
/*
 * Auth: Yoann Mailland
 * Date: 2016 04 18
 * Copyright (c) 2016, Yoann Mailland
 * Info: Define missing delcarations of functions prototypes
 *       for Tensilica XTENSA ESP8266
 */


#ifndef __XT_MISSING__
#define __XT_MISSING__

#include <stddef.h>

#define free(ptr)       (vPortFree(ptr, "", 0))
void vPortFree(void *p, char*, uint16 len);

#define malloc(s)       (pvPortMalloc(s, "", 0))
void *pvPortMalloc(size_t s, char*, uint16 len);

#define zalloc(s)       (pvPortZalloc(s, "", 0))
void *pvPortZalloc(size_t s, char*, uint16 len);

#define memcpy ets_memcpy
void *ets_memcpy(void *dest, const void *src, size_t len);

#define udelay ets_delay_us
void ets_delay_us(long us);

#define sprintf ets_sprintf
int ets_sprintf(char *str, const char *format, ...)
        __attribute__ ((format (__printf__, 2, 3)));

#define printf os_printf
int os_printf(const char *format, ...) 
        __attribute__ ((format (__printf__, 1, 2)));

#define snprintf os_snprintf
int os_snprintf(char *str, size_t size, const char *format, ...)
        __attribute__ ((format (__printf__, 3, 4)));

#define printf_plus os_printf_plus
int os_printf_plus(const char *format, ...)
        __attribute__ ((format (__printf__, 1, 2)));



//
// Redefine some macro
// To avoid warning
//

// Add parentheses (eagle_soc.h:250)
#undef PIN_FUNC_SELECT
#define PIN_FUNC_SELECT(PIN_NAME, FUNC)  do { \
    WRITE_PERI_REG(PIN_NAME,   \
        (READ_PERI_REG(PIN_NAME) \
             &  (~(PERIPHS_IO_MUX_FUNC<<PERIPHS_IO_MUX_FUNC_S)))  \
             |( (((FUNC&BIT2)<<2)|(FUNC&0x3))<<PERIPHS_IO_MUX_FUNC_S) );  \
    } while (0)




#endif // __XT_MISSING__
