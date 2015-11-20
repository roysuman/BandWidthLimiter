SHELL:=/bin/bash
READLINK_ARGS:="-f"
PLATFORM_COPTS:=-mfpmath=sse
PLATFORM_WARNINGS:=-Wframe-larger-than=16384 -Wno-unused-but-set-variable \
	-Wunused-but-set-parameter -Wvla -Wno-builtin-macro-redefined
HEADERS:=include
CC=gcc
LDFLAGS=-Wl,--fatal-warnings
AGENT=libc_band.so
LIBS=-ldl
RELEASE=32
BUILD_DIR ?= $(shell mkdir build-$(RELEASE) 2> /dev/null ; echo build-$(RELEASE))
SRC_DIR:=${PWD}/src
OPT?=-O
GLOBAL_WARNINGS=-Wall -Werror -Wformat-security -Wno-char-subscripts \
	-Wno-sign-compare -Wno-strict-overflow -Wwrite-strings
GLOBAL_COPTS=-fdiagnostics-show-option -fno-exceptions \
	-fno-omit-frame-pointer -fno-strict-aliasing -funsigned-char \
	-fno-asynchronous-unwind-tables -msse2 -g \
	-D__STDC_FORMAT_MACROS
COPTS:=$(PLATFORM_COPTS) $(GLOBAL_COPTS) $(PLATFORM_WARNINGS) \
	$(GLOBAL_WARNINGS) $(OPT)

#INCLUDES=-I$(JAVA_HOME)/$(HEADERS) -I$(JAVA_HOME)/$(HEADERS)/$(UNAME)


# LDFLAGS+=-Wl,--export-dynamic-symbol=Agent_OnLoad

SOURCES=$(wildcard $(SRC_DIR)/*.c)
_OBJECTS=$(SOURCES:.c=.pic.o)
OBJECTS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(_OBJECTS))

$(BUILD_DIR)/%.pic.o: $(SRC_DIR)/%.c
	$(CC) $(COPTS) -Fvisibility=hidden -fPIC -c $< -o $@

$(AGENT): $(OBJECTS)
	$(CC) $(COPTS) -shared -o $(BUILD_DIR)/$(AGENT) \
	  -Bsymbolic $(OBJECTS) $(LIBS)

all: $(AGENT)

clean:
	rm -rf $(BUILD_DIR)/*
