#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bus.h"

struct tty_state {
    char tx;
};

struct tty_state* tty_create() {
    return malloc(sizeof(struct tty_state));
}

void tty_init(struct tty_state* tty) {
    tty->tx = 0;
}

uint32_t tty_read32(void* udata, uint32_t addr) { return 0; }
uint32_t tty_read16(void* udata, uint32_t addr) { return 0; }
uint32_t tty_read8(void* udata, uint32_t addr) { return 0; }

void tty_write32(void* udata, uint32_t addr, uint32_t data) {
    putchar(data);
}

void tty_write16(void* udata, uint32_t addr, uint32_t data) {
    putchar(data);
}

void tty_write8(void* udata, uint32_t addr, uint32_t data) {
    putchar(data);
}

void tty_destroy(struct tty_state* tty) {
    free(tty);
}

void tty_init_bus_device(struct tty_state* tty, struct bus_device* dev) {
    dev->read32 = tty_read32;
    dev->read16 = tty_read16;
    dev->read8 = tty_read8;
    dev->write32 = tty_write32;
    dev->write16 = tty_write16;
    dev->write8 = tty_write8;
    dev->udata = tty;
}