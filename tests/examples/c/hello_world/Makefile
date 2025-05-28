CC ?= gcc
CFLAGS += -g -Wall -Wextra -std=c17 -O0 -D_GNU_SOURCE
# Add ASan flags to match the library
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer
RED_INSTALL_PATH ?= /opt/ddn/red
CUR_DIR := $(abspath .)
SDK_ROOT := $(CUR_DIR)/../../..
SDK_C_INCLUDE := $(SDK_ROOT)/c/include
COMMON_LIB := $(CUR_DIR)/../common
# Build the common library first
.PHONY: common_lib
common_lib:
	$(MAKE) -C $(COMMON_LIB)

INCLUDES += -I$(SDK_C_INCLUDE)

# Conditionally include RED build path if RED environment variable is set
ifdef RED
LIBS = -L$(RED)/pkgbuild/red_inst/lib -L$(RED_INSTALL_PATH)/lib -L$(COMMON_LIB) -lcommon -lred_client
else
LIBS = -L$(RED_INSTALL_PATH)/lib -L$(COMMON_LIB) -lcommon -lred_client
endif

TARGET = hello-world
SRCS = hello_world.c

.DEFAULT_GOAL := all

.PHONY: all
all: $(TARGET) compile_commands

$(TARGET): common_lib $(SRCS)
	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -o $@ $(SRCS) $(LIBS)

.PHONY: compile_commands
compile_commands:
	@echo '[' > compile_commands.json
	@echo '  {' >> compile_commands.json
	@echo '    "directory": "$(shell pwd)",' >> compile_commands.json
	@echo '    "command": "$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -c -o $(TARGET).o $(SRCS)",' >> compile_commands.json
	@echo '    "file": "$(shell pwd)/$(SRCS)",' >> compile_commands.json
	@echo '    "output": "$(TARGET).o"' >> compile_commands.json
	@echo '  }' >> compile_commands.json
	@echo ']' >> compile_commands.json

.PHONY: clean
clean:
	$(MAKE) -C $(COMMON_LIB) clean
	$(RM) $(TARGET) compile_commands.json
