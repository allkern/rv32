#include "ram.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct ram_state* ram_create() {
    return (struct ram_state*)malloc(sizeof(struct ram_state));
}

void ram_init(struct ram_state* ram, size_t size) {
    memset(ram, 0, sizeof(struct ram_state));

    ram->buf = malloc(size);
}

uint32_t ram_read32(void* udata, uint32_t addr) {
    struct ram_state* ram = (struct ram_state*)udata;

    return *(uint32_t*)(&ram->buf[addr]);
}

uint32_t ram_read16(void* udata, uint32_t addr) {
    struct ram_state* ram = (struct ram_state*)udata;

    return *(uint16_t*)(&ram->buf[addr]);
}

uint32_t ram_read8(void* udata, uint32_t addr) {
    struct ram_state* ram = (struct ram_state*)udata;

    return *(uint8_t*)(&ram->buf[addr]);
}

void ram_write32(void* udata, uint32_t addr, uint32_t data) {
    struct ram_state* ram = (struct ram_state*)udata;

    *(uint32_t*)(&ram->buf[addr]) = data;
}

void ram_write16(void* udata, uint32_t addr, uint32_t data) {
    struct ram_state* ram = (struct ram_state*)udata;

    *(uint16_t*)(&ram->buf[addr]) = data;
}

void ram_write8(void* udata, uint32_t addr, uint32_t data) {
    struct ram_state* ram = (struct ram_state*)udata;

    *(uint8_t*)(&ram->buf[addr]) = data;
}

void ram_destroy(struct ram_state* ram) {
    free(ram->buf);
    free(ram);
}

void ram_init_bus_device(struct ram_state* ram, struct bus_device* dev) {
    dev->read32 = ram_read32;
    dev->read16 = ram_read16;
    dev->read8 = ram_read8;
    dev->write32 = ram_write32;
    dev->write16 = ram_write16;
    dev->write8 = ram_write8;
    dev->udata = ram;
}