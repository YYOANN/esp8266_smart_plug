/*
 * params_test.h
 *
 *  Created on: May 26, 2013
 *      Author: petera
 */

#ifndef SPIFFS_PARAMS_H_
#define SPIFFS_PARAMS_H_

/////////// SPIFFS BUILD CONFIG  ////////////

// test using filesystem magic
#ifndef SPIFFS_USE_MAGIC
#define SPIFFS_USE_MAGIC    1
#endif
// test using filesystem magic length
#ifndef SPIFFS_USE_MAGIC_LENGTH
#define SPIFFS_USE_MAGIC_LENGTH   1
#endif
// test using extra param in callback
#ifndef SPIFFS_HAL_CALLBACK_EXTRA
#define SPIFFS_HAL_CALLBACK_EXTRA       1
#endif
// test using filehandle offset
#ifndef SPIFFS_FILEHDL_OFFSET
#define SPIFFS_FILEHDL_OFFSET           1
// use this offset
#define TEST_SPIFFS_FILEHDL_OFFSET      0x1000
#endif

#ifdef NO_TEST
#define SPIFFS_LOCK(fs)
#define SPIFFS_UNLOCK(fs)
#else
struct spiffs_t;
extern void test_lock(struct spiffs_t *fs);
extern void test_unlock(struct spiffs_t *fs);
#define SPIFFS_LOCK(fs)   test_lock(fs)
#define SPIFFS_UNLOCK(fs) test_unlock(fs)
#endif

// dbg output
#define SPIFFS_DBG(...) //printf(__VA_ARGS__)
#define SPIFFS_GC_DBG(...) //printf(__VA_ARGS__)
#define SPIFFS_CACHE_DBG(...) //printf(__VA_ARGS__)
#define SPIFFS_CHECK_DBG(...) //printf(__VA_ARGS__)

// needed types
typedef signed int s32_t;
typedef unsigned int u32_t;
typedef signed short s16_t;
typedef unsigned short u16_t;
typedef signed char s8_t;
typedef unsigned char u8_t;

#endif // SPIFFS_PARAMS_H_
