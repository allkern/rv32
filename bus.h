#ifndef BUS_H
#define BUS_H

#include <stdint.h>

typedef uint32_t (*dev_read32)(void* udata, uint32_t addr);
typedef uint32_t (*dev_read16)(void* udata, uint32_t addr);
typedef uint32_t (*dev_read8)(void* udata, uint32_t addr);
typedef void (*dev_write32)(void* udata, uint32_t addr, uint32_t data);
typedef void (*dev_write16)(void* udata, uint32_t addr, uint32_t data);
typedef void (*dev_write8)(void* udata, uint32_t addr, uint32_t data);

struct bus_device {
    uint32_t io_start;
    uint32_t io_end;
    dev_read32 read32;
    dev_read16 read16;
    dev_read8 read8;
    dev_write32 write32;
    dev_write16 write16;
    dev_write8 write8;
    void* udata;
};

struct bus_device* bus_device_create();
void bus_device_init(struct bus_device*, uint32_t, uint32_t);
void bus_device_destroy(struct bus_device*);

struct bus_state {
    struct bus_device** devices;
    unsigned device_count;
};

struct bus_state* bus_create();
void bus_init(struct bus_state* bus);
struct bus_device* bus_register_device(struct bus_state* bus, uint32_t, uint32_t);
void bus_destroy(struct bus_state* bus);
uint32_t bus_read32(void* udata, uint32_t addr);
uint32_t bus_read16(void* udata, uint32_t addr);
uint32_t bus_read8(void* udata, uint32_t addr);
void bus_write32(void* udata, uint32_t addr, uint32_t data);
void bus_write16(void* udata, uint32_t addr, uint32_t data);
void bus_write8(void* udata, uint32_t addr, uint32_t data);

#endif

