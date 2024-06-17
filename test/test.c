#include "mmio.h"
#include "tty.h"

void putchar(char c) {
    mmio_write_8(TTY_PUT, c);
}

void puts(const char* str) {
    while (*str)
        putchar(*str++);
}

void main() {
    puts("Hello, world!\n");
}

void _start() {
    float a = 1.0f;
    float b = 2.0f;
    float c = (a * b) + 4.0f;

    // main();

    asm volatile ("ebreak");
}