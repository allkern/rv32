#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include <stdlib.h>

#include "bus.h"

struct ram_state {
    uint8_t* buf;
};

struct ram_state* ram_create();
void ram_init(struct ram_state*, size_t);
uint32_t ram_read32(void*, uint32_t);
uint32_t ram_read16(void*, uint32_t);
uint32_t ram_read8(void*, uint32_t);
void ram_write32(void*, uint32_t, uint32_t);
void ram_write16(void*, uint32_t, uint32_t);
void ram_write8(void*, uint32_t, uint32_t);
void ram_destroy(struct ram_state*);
void ram_init_bus_device(struct ram_state*, struct bus_device*);

#endif