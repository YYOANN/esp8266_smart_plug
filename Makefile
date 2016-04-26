#
# Auth: Yoann Mailland
# Date: 2016 04 18
# Copyright (c) 2016, Yoann Mailland
# Info: Makefile to build and flash EPS8622
 


TARGET		:= image.elf
LIB_DRV		:= libdriver.a
FW_FILE_1	:= 0x00000.bin
FW_FILE_2	:= 0x40000.bin


all		: $(TARGET)
driver		: $(LIB_DRV)
firmware	: $(FW_FILE_1) $(FW_FILE_2)


SRC		:= main.c http.c http_handlers.c http_utils.c
LIB_DRV_SRC	:= gpio16.c hw_timer.c i2c_master.c key.c spi.c spi_overlap.c uart.c
INCLUDE		:= ./include
LIBS		:= c gcc hal phy pp net80211 lwip wpa main json upgrade ssl
DRV_LIB		:= driver	# WARN: depends of the name of the lib
LD_SCRIPT	:= eagle.app.v6.ld
OBJ_DIR		:= obj




SDK_DIR		:= [SDK_PATH]/esp-open-sdk
SDK		:= $(SDK_DIR)/esp_iot_sdk_v1.4.0
FW_TOOL		:= [ESPTOOL_CK_PATH]/esptool
ESPTOOL_PY	:= $(SDK_DIR)/esptool/esptool.py
PORT		:= /dev/ttyUSB0
BD_RATE		:= 115200


XT_GCCLIB	:= $(SDK_DIR)/xtensa-lx106-elf/lib/gcc/xtensa-lx106-elf/4.8.2/
XT_CLIB		:= $(SDK_DIR)/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/
XT_LIB		:= $(SDK_DIR)/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr/lib/
XT_SCRIPT_DIR	:= $(SDK)/ld/
LIB_DRV_PATH	:= ./
CROSS_COMPILE	:= $(SDK_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-


CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
AR		:= $(CROSS_COMPILE)ar


LIBS		:= $(addprefix -l, $(LIBS))
DRV_LIB		:= $(addprefix -l, $(DRV_LIB))
INCLUDE		:= $(addprefix -I, $(INCLUDE))

LIB_DRV_OBJ_	:= $(patsubst %.c, %.o, $(LIB_DRV_SRC))
LIB_DRV_OBJ	:= $(addprefix $(OBJ_DIR)/, $(LIB_DRV_OBJ_))
OBJ_		:= $(patsubst %.c, %.o, $(SRC))
OBJ		:= $(addprefix $(OBJ_DIR)/, $(OBJ_))

LIB_DRV_SRC	:= $(addprefix driver/, $(LIB_DRV_SRC))




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
		-L$(XT_CLIB) \
		-L$(XT_GCCLIB) \
		-L$(XT_LIB) \
		-L$(XT_SCRIPT_DIR) \
		-L$(LIB_DRV_PATH) \
		-T$(LD_SCRIPT)



# Firmware: sections to copy
FW_FILE_1_ARGS	:= -bs .text -bs .data -bs .rodata
FW_FILE_2_ARGS	:= -es .irom0.text



# BUILD APP
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $^ -c
	mv $(OBJ_) $(OBJ_DIR)


# LINK APP
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) --start-group $(LIBS) --end-group $^ -o $@ $(DRV_LIB)


# BUILD DRIVER
$(LIB_DRV_OBJ): $(LIB_DRV_SRC)
	$(CC) $(CFLAGS) -w $^ -c
	mv $(LIB_DRV_OBJ_) $(OBJ_DIR)


# ARCHIVE DRIVER
$(LIB_DRV): $(LIB_DRV_OBJ)
	$(AR) cru $@ $^


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


clean-all: clean clean-driver
