.PHONY: clean

CROSS_PREFIX := riscv32-linux

CC := gcc
SOURCES := $(wildcard *.c)
CFLAGS := -march=rv32g -mabi=ilp32
CFLAGS += -ffreestanding -nostdlib
CFLAGS += -fno-pie -no-pie

test.elf test.c:
	$(CROSS_PREFIX)-$(CC) $(SOURCES) -o test.elf $(CFLAGS)

clean:
	rm $(wildcard *.elf)