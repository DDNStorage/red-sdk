CXX = g++
CXXFLAGS = -g -Wall -Wextra -fPIC -D_GNU_SOURCE -O0 -std=c++17
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer

RED_INSTALL_PATH = /opt/ddn/red
CUR_DIR := $(abspath .)
SDK_ROOT = $(CUR_DIR)/../../../..
SDK_C_INCLUDE = $(SDK_ROOT)/sdk/c/include
COMMON_LIB = $(SDK_ROOT)/sdk/examples/cpp/common
SIMPLE_S3_DIR = $(SDK_ROOT)/sdk/examples/cpp/simple_s3
INCLUDES = -I$(SDK_C_INCLUDE) \
	-I$(SDK_C_INCLUDE)/red \
	-I$(RED_INSTALL_PATH)/include \
	-I$(COMMON_LIB)/include \
	-I$(SIMPLE_S3_DIR) \
	-I/usr/include/gtest \
	-I/usr/include/gmock

# Conditionally include RED build path if RED environment variable is set
ifdef RED
LIBS = -L$(RED)/pkgbuild/red_inst/lib -L$(RED_INSTALL_PATH)/lib -L$(COMMON_LIB) \
	-lcommon -lred_client -lgtest -lgmock -lpthread
LDFLAGS = -Wl,-rpath,$(RED)/pkgbuild/red_inst/lib
else
LIBS = -L$(RED_INSTALL_PATH)/lib -L$(COMMON_LIB) \
	-lcommon -lred_client -lgtest -lgmock -lpthread
LDFLAGS = -Wl,-rpath,$(RED_INSTALL_PATH)/lib
endif

TARGET = cpp-unit-test
SRCS = $(wildcard *.cpp) $(SIMPLE_S3_DIR)/simple_s3_client.cpp

# Build the common library first
.PHONY: common_lib
common_lib:
	$(MAKE) -C $(COMMON_LIB)

.DEFAULT_GOAL := all

.PHONY: all
all: common_lib $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(ASAN_FLAGS) $(INCLUDES) -o $@ $^ $(LIBS) $(LDFLAGS)

.PHONY: clean
clean:
	$(MAKE) -C $(COMMON_LIB) clean
	$(RM) $(TARGET)
