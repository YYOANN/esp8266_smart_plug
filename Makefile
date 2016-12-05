#
# Auth: Yoann Mailland
# Date: 2016 04 18
# Copyright (c) 2016, Yoann Mailland
# Info: Makefile to build and flash EPS8622



TARGET		:= image.elf
LIB_DRV		:= libdriver.a
LIB_SPIFFS	:= libspiffs.a
FW_FILE_1	:= 0x00000.bin
FW_FILE_2	:= 0x40000.bin


all		: $(LIB_DRV) $(LIB_SPIFFS) $(TARGET) $(FW_FILE_1) $(FW_FILE_2)
driver		: $(LIB_DRV)
spiffs		: $(LIB_SPIFFS)
firmware	: $(FW_FILE_1) $(FW_FILE_2)
elf		: $(TARGET)


SRC		:= main.c http.c http_handlers.c http_utils.c
LIB_DRV_SRC	:= gpio16.c hw_timer.c i2c_master.c key.c spi.c spi_overlap.c uart.c
LIB_SPIFFS_SRC	:= spiffs_cache.c spiffs_check.c spiffs_gc.c spiffs_hydrogen.c spiffs_nucleus.c
INCLUDE		:= ./include
SPIFFS_INCLUDE	:= ./include
LIBS		:= c gcc hal phy pp net80211 lwip wpa main json upgrade ssl
DRV_LIB		:= $(LIB_DRV:lib%.a=%)
SPIFFS_LIB	:= $(LIB_SPIFFS:lib%.a=%)
LD_SCRIPT	:= eagle.app.v6.ld
OBJ_DIR		:= obj




SDK_DIR		:= [SDK_PATH]/esp-open-sdk
SDK		:= $(SDK_DIR)/sdk/
SPIFFS_DIR	:= $(PWD)/spiffs/src/
FW_TOOL		:= [ESPTOOL_CK_PATH]/esptool
ESPTOOL_PY	:= $(SDK_DIR)/esptool/esptool.py
PORT		:= /dev/ttyUSB0
BD_RATE		:= 115200


XT_GCCLIB	:= $(SDK_DIR)/xtensa-lx106-elf/lib/gcc/xtensa-lx106-elf/4.8.5/
XT_CLIB		:= $(SDK_DIR)/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/
XT_LIB		:= $(SDK_DIR)/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr/lib/
SDK_LIB		:= $(SDK)/lib/
XT_SCRIPT_DIR	:= $(SDK)/ld/
LIB_DRV_PATH	:= $(PWD)
CROSS_COMPILE	:= $(SDK_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-


CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
AR		:= $(CROSS_COMPILE)ar
OBJCOPY		:= $(CROSS_COMPILE)objcopy


LIBS		:= $(addprefix -l, $(LIBS))
DRV_LIB_FL	:= $(addprefix -l, $(DRV_LIB))
SPIFFS_LIB_FL	:= $(addprefix -l, $(SPIFFS_LIB))
INCLUDE		:= $(addprefix -I, $(INCLUDE))

LIB_DRV_OBJ_	:= $(patsubst %.c, %.o, $(LIB_DRV_SRC))
LIB_DRV_OBJ	:= $(addprefix $(OBJ_DIR)/, $(LIB_DRV_OBJ_))
LIB_SPIFFS_OBJ_	:= $(patsubst %.c, %.o, $(LIB_SPIFFS_SRC))
LIB_SPIFFS_OBJ	:= $(addprefix $(OBJ_DIR)/, $(LIB_SPIFFS_OBJ_))
OBJ_		:= $(patsubst %.c, %.o, $(SRC))
OBJ		:= $(addprefix $(OBJ_DIR)/, $(OBJ_))

LIB_DRV_SRC	:= $(addprefix driver/, $(LIB_DRV_SRC))
LIB_SPIFFS_SRC	:= $(addprefix $(SPIFFS_DIR)/, $(LIB_SPIFFS_SRC))


CFLAGS		:= \
		-I$(SDK)/include/ \
		$(INCLUDE) \
		-Wall \
		-Werror \
		-nostdlib \
		-mlongcalls \
		-fno-inline-functions \
		-mtext-section-literals \
		-Os

LDFLAGS		:=\
		-nostdlib \
		-flto \
		-u call_user_start \
		-static \
		-L$(PWD) \
		-L$(XT_CLIB) \
		-L$(XT_GCCLIB) \
		-L$(XT_LIB) \
		-L$(SDK_LIB) \
		-L$(XT_SCRIPT_DIR) \
		-T$(LD_SCRIPT)


SPIFFS_CFLAGS = \
-Wall -Wno-format-y2k -W -Wstrict-prototypes -Wmissing-prototypes \
-Wpointer-arith -Wreturn-type -Wcast-qual -Wwrite-strings -Wswitch \
-Wshadow -Wcast-align -Wchar-subscripts -Winline -Wnested-externs\
-Wredundant-decls

# Firmware: sections to copy
FW_FILE_1_ARGS	:= -bs .text -bs .data -bs .rodata
FW_FILE_2_ARGS	:= -es .irom0.text

# Move code to falsh to save memory
MOVE_TO_FLASH	:= $(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal

# BUILD APP
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $^ -c
	mv $(OBJ_) $(OBJ_DIR)


# LINK APP
$(TARGET): $(OBJ)
	$(foreach OBJ_FILE, $^, \
		$(MOVE_TO_FLASH) $(OBJ_FILE);)
	$(LD) $(LDFLAGS) --start-group $(LIBS) --end-group $^ -o $@ $(DRV_LIB_FL) $(SPIFFS_LIB_FL)


# BUILD DRIVER
$(LIB_DRV_OBJ): $(LIB_DRV_SRC)
	$(CC) $(CFLAGS) -w $^ -c
	mv $(LIB_DRV_OBJ_) $(OBJ_DIR)


# ARCHIVE DRIVER
$(LIB_DRV): $(LIB_DRV_OBJ)
	$(AR) cru $@ $^
	$(MOVE_TO_FLASH) $@


# BUILD SPIFFS LIB
$(LIB_SPIFFS_OBJ): $(LIB_SPIFFS_SRC)
	$(CC) $(SPIFFS_CFLAGS) -I$(PWD) -I$(SPIFFS_DIR) -w $^ -c
	mv $(LIB_SPIFFS_OBJ_) $(OBJ_DIR)


# ARCHIVE SPIFFS LIB
$(LIB_SPIFFS): $(LIB_SPIFFS_OBJ)
	$(AR) cru $@ $^
	$(MOVE_TO_FLASH) $@


# FIRMWARE
$(FW_FILE_1): $(TARGET)
	$(FW_TOOL) -eo $(TARGET) -bo $@ $(FW_FILE_1_ARGS) -bc -ec

$(FW_FILE_2): $(TARGET)
	$(FW_TOOL) -eo $(TARGET) $(FW_FILE_2_ARGS) $@ -ec


# Copy to device
burn: $(FW_FILE_1) $(FW_FILE_2)
	($(ESPTOOL_PY) --port $(PORT) --baud $(BD_RATE) write_flash 0x00000 0x00000.bin 0x40000 0x40000.bin)||(true)


clean :
	rm -rf $(TARGET)
	rm -rf $(OBJ)
	rm -rf $(FW_FILE_1)
	rm -rf $(FW_FILE_2)

clean-driver:
	rm -rf $(LIB_DRV)
	rm -rf $(LIB_DRV_OBJ)

clean-spiffs:
	rm -rf $(LIB_SPIFFS)
	rm -rf $(LIB_SPIFFS_OBJ)

clean-all: clean clean-driver clean-spiffs

