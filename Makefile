CC = gcc
#CFLAGS = -Wall -Wextra -std=c11 -O2 -g -mavx2
CFLAGS = -O0 -mavx2
INCLUDES = -Isrc
LDFLAGS = -lm

SRC_DIR = src
BUILD_DIR = build
CODE_DIR = code

LIB_DIR = libs
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

LIB_FILES = $(wildcard $(LIB_DIR)/*.c)
SO_FILES = $(patsubst $(LIB_DIR)/%.c,$(BIN_DIR)/%.so,$(LIB_FILES))

TARGET = $(BUILD_DIR)/Main

all:
	$(CC) $(CFLAGS) $(INCLUDES) ./$(SRC_DIR)/Main.c -o ./$(TARGET) $(LDFLAGS) 

clean:
	rm -rf $(BUILD_DIR)/*

lib: $(SO_FILES)

$(BIN_DIR)/%.so: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -fPIC -o $@ $< -shared $(LDFLAGS)

cleanlib:
	rm -rf $(BIN_DIR)/*

exe:
	./$(TARGET) ./code/Main.iralx

dg: clean
	$(CC) $(CFLAGS) -g $(INCLUDES) ./$(SRC_DIR)/Main.c -o ./$(TARGET) $(LDFLAGS)
	-./$(TARGET) ./code/Main.iralx
	ulimit -c unlimited
#sudo echo "./core" > /proc/sys/kernel/core_pattern
	gdb ./$(TARGET) ./core

#nasm -f elf32 hello.asm -o hello.o
#ld -m elf_i386 hello.o -o hello

do: clean all exe