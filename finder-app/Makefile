CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc
CFLAGS ?= -Wall -Wextra -g

OUTPUT_DIR := ../build
TARGET := $(OUTPUT_DIR)/writer
OBJS := $(OUTPUT_DIR)/writer.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OUTPUT_DIR)/writer.o: writer.c
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OUTPUT_DIR)/*.o
