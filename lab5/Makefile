BUILD_ROOT=../..

INCLUDE_DIRS += -I./include
SRC_DIR = ./src
SRCS = $(wildcard  $(addsuffix /*.c, $(SRC_DIR)))
OBJS = $(patsubst %.c, %.o, $(SRCS))

LIB_DIR = ./lib
LIB_SRC_DIR = ./lib_src
LIB = $(LIB_DIR)/libled.a
LIB_OBJS = $(LIB_SRC_DIR)/led.o
LIB_DIRS  += $(LIB_DIR)/
LDLIBS += -lled

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(LIB) $(BINARY_NAME).bin

library: $(LIB)

$(LIB): $(LIB_OBJS)
	mkdir -p $(@D)
	@echo "Linking library $@"
	$(MAKE_LIBRARY)


clean:
	@rm -f $(LIB_SRC_DIR)/*.o $(LIB_SRC_DIR)/*.d $(LIB) \
	$(SRC_DIR)/*.o $(SRC_DIR)/*.d \
	main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)