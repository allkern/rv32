#include "bus.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

struct bus_device* bus_device_create() {
    return (struct bus_device*)malloc(sizeof(struct bus_device));
}

void bus_device_init(struct bus_device* dev, uint32_t io_start, uint32_t io_end) {
    memset(dev, 0, sizeof(struct bus_device));

    dev->io_start = io_start;
    dev->io_end = io_end;
}

void bus_device_destroy(struct bus_device* dev) {
    free(dev);
}

struct bus_state* bus_create() {
    return (struct bus_state*)malloc(sizeof(struct bus_state));
}

void bus_init(struct bus_state* bus) {
    memset(bus, 0, sizeof(struct bus_state));

    bus->devices = NULL;
    bus->device_count = 0;
}

struct bus_device* bus_register_device(struct bus_state* bus, uint32_t io_start, uint32_t io_end) {
    struct bus_device* dev = bus_device_create();

    bus_device_init(dev, io_start, io_end);

    if (!bus->devices) {
        bus->devices = malloc(sizeof(struct bus_device*));
    } else {
        bus->devices = realloc(bus->devices, sizeof(struct bus_device*) * (1 + bus->device_count));
    }

    bus->devices[bus->device_count++] = dev;

    return dev;
}

struct bus_device* search_device(struct bus_state* bus, uint32_t addr) {
    for (int i = 0; i < bus->device_count; i++) {
        struct bus_device* dev = bus->devices[i];

        if ((addr >= dev->io_start) && (addr < dev->io_end))
            return dev;
    }

    return NULL;
}

void bus_destroy(struct bus_state* bus) {
    for (int i = 0; i < bus->device_count; i++) {
        bus_device_destroy(bus->devices[i]);
    
        free(bus->devices[i]);
    }

    free(bus->devices);
    free(bus);
}

uint32_t bus_read32(void* udata, uint32_t addr) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return 0xdeadc0de;

    return dev->read32(dev->udata, addr - dev->io_start);
}

uint32_t bus_read16(void* udata, uint32_t addr) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return 0xbaad;

    return dev->read16(dev->udata, addr - dev->io_start);
}

uint32_t bus_read8(void* udata, uint32_t addr) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return 0xf0;

    return dev->read8(dev->udata, addr - dev->io_start);
}

void bus_write32(void* udata, uint32_t addr, uint32_t data) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return;

    dev->write32(dev->udata, addr - dev->io_start, data);
}

void bus_write16(void* udata, uint32_t addr, uint32_t data) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return;

    dev->write16(dev->udata, addr - dev->io_start, data);
}

void bus_write8(void* udata, uint32_t addr, uint32_t data) {
    struct bus_state* bus = (struct bus_state*)udata;
    struct bus_device* dev = search_device(bus, addr);

    // Bus error
    if (!dev)
        return;

    dev->write8(dev->udata, addr - dev->io_start, data);
}