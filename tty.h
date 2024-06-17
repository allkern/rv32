#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stdlib.h>

#include "bus.h"

struct tty_state {
    char tx;
};

struct tty_state* tty_create();
void tty_init(struct tty_state* tty);
uint32_t tty_read32(void* udata, uint32_t addr);
uint32_t tty_read16(void* udata, uint32_t addr);
uint32_t tty_read8(void* udata, uint32_t addr);
void tty_write32(void* udata, uint32_t addr, uint32_t data);
void tty_write16(void* udata, uint32_t addr, uint32_t data);
void tty_write8(void* udata, uint32_t addr, uint32_t data);
void tty_destroy(struct tty_state* tty);
void tty_init_bus_device(struct tty_state* tty, struct bus_device* bdev);

#endif