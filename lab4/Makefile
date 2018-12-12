BUILD_ROOT=../..

INCLUDE_DIRS += -I./include
SRC_DIR = ./src
SRCS = $(wildcard  $(addsuffix /*.c, $(SRC_DIR)))
OBJS = $(patsubst %.c, %.o, $(SRCS))

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.d \
	main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)
	
info:
	@echo $(SRCS)	
	@echo $(OBJS)