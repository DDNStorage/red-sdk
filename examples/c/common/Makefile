CC ?= gcc
AR ?= ar
RANLIB ?= ranlib

CUR_DIR := $(abspath .)
SDK_ROOT := $(CUR_DIR)/../../..
SDK_C_INCLUDE := $(SDK_ROOT)/c/include

CFLAGS += -g -Wall -Wextra -Werror -std=gnu17 -O0 -D_GNU_SOURCE
# Add ASan flags to match the library
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer

INCLUDES += -I$(SDK_C_INCLUDE) -I$(CUR_DIR)/include

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
LIB := libcommon.a

.DEFAULT_GOAL := all

.PHONY: all
all: $(LIB) compile_commands

%.o: %.c
	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

.PHONY: clean
clean:
	$(RM) $(OBJS) $(LIB) compile_commands.json

.PHONY: compile_commands
compile_commands:
	@echo '[' > compile_commands.json
	@for src in $(SRCS); do \
		echo '  {' >> compile_commands.json; \
		echo '    "directory": "$(shell pwd)",' >> compile_commands.json; \
		echo '    "command": "$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -c -o '$${src%.*}'.o $$src",' >> compile_commands.json; \
		echo '    "file": "$(shell pwd)/$$src",' >> compile_commands.json; \
		echo '    "output": "'$${src%.*}'.o"' >> compile_commands.json; \
		echo '  },' >> compile_commands.json; \
	done
	@sed -i '$$s/,$$//' compile_commands.json
	@echo ']' >> compile_commands.json
