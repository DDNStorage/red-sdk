CXX ?= g++
AR ?= ar
RANLIB ?= ranlib

CUR_DIR := $(abspath .)
SDK_ROOT := $(CUR_DIR)/../../..
SDK_C_INCLUDE := $(SDK_ROOT)/c/include

CXXFLAGS += -g -Wall -Wextra -Werror -std=c++17 -fno-exceptions -O0
# Add ASan flags to match the library
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer

INCLUDES += -I$(SDK_C_INCLUDE)

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
LIB := libcommon.a

.DEFAULT_GOAL := all

.PHONY: all
all: $(LIB) compile_commands

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(ASAN_FLAGS) $(INCLUDES) -c $< -o $@

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
		echo '    "command": "$(CXX) $(CXXFLAGS) $(ASAN_FLAGS) $(INCLUDES) -c -o '$${src%.*}'.o $$src",' >> compile_commands.json; \
		echo '    "file": "$(shell pwd)/$$src",' >> compile_commands.json; \
		echo '    "output": "'$${src%.*}'.o"' >> compile_commands.json; \
		echo '  },' >> compile_commands.json; \
	done
	@sed -i '$$s/,$$//' compile_commands.json
	@echo ']' >> compile_commands.json
