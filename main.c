#include <stdio.h>

#include "rv32.h"
#include "bus.h"
#include "ram.h"

#define RAM_START 0x80400000
#define RAM_SIZE 0x1000000

int main() {
    struct rv32_state* cpu = rv32_create();
    struct bus_state* bus = bus_create();
    struct ram_state* ram = ram_create();

    bus_init(bus);

    struct bus_device* ram_bdev = bus_register_device(bus, RAM_START, RAM_START + RAM_SIZE);
    
    ram_init(ram, RAM_SIZE);
    
    ram_init_bus_device(ram, ram_bdev);

    struct rv32_bus cpu_bus = {
        .read32 = bus_read32,
        .read16 = bus_read16,
        .read8 = bus_read8,
        .write32 = bus_write32,
        .write16 = bus_write16,
        .write8 = bus_write8,
        .udata = bus
    };

    rv32_init(cpu, cpu_bus);

    FILE* file = fopen("test.bin", "rb");
    
    fread(ram->buf, 1, RAM_SIZE, file);
    fclose(file);

    rv32_set_pc(cpu, RAM_START);

    for (int i = 0; i < 8; i++)
        rv32_cycle(cpu);

    ram_destroy(ram);
    bus_destroy(bus);
    rv32_destroy(cpu);
}