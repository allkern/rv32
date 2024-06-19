#include <filesystem>
#include <iostream>
#include <fstream>

extern "C" {
#include "rv32.h"
#include "bus.h"
#include "ram.h"
#include "tty.h"
#include "elf_ldr.h"
}

#define RAM_START 0x80000000
#define TTY_START 0xa0000000
#define RAM_SIZE 0x1000000
#define TTY_SIZE 0x8

struct rv32_state* cpu;
struct ram_state* ram;

int run_test(std::string path) {
    elf_file_t* elf = elf_create();

    if (elf_load(elf, path.c_str()))
        return 1;

    for (int i = 0; i < elf->ehdr->e_phnum; i++) {
        Elf32_Phdr* phdr = elf->phdr[i];

        if (elf->phdr[i]->p_type != PT_LOAD)
            continue;

        elf_load_segment(elf, i, &ram->buf[phdr->p_paddr - RAM_START]);

        // Initialize stack
        cpu->x[2] = RAM_START + RAM_SIZE;

        // Set PC to entry point
        rv32_set_pc(cpu, elf->ehdr->e_entry);

        while (1) {
            cpu->opcode = rv32_bus_read32(cpu, cpu->pc);

            if (cpu->opcode == 0x00000073) {
                if (cpu->x[17] != 93) 
                    continue;

                return cpu->x[10] == 0;
            }

            rv32_cycle(cpu);
        }
    }

    return 0;
}

int main(int argc, const char* argv[]) {
    cpu = rv32_create();
    ram = ram_create();

    struct bus_state* bus = bus_create();
    struct tty_state* tty = tty_create();

    bus_init(bus);

    struct bus_device* ram_bdev = bus_register_device(bus, RAM_START, RAM_START + RAM_SIZE);
    struct bus_device* tty_bdev = bus_register_device(bus, TTY_START, TTY_START + TTY_SIZE);

    ram_init(ram, RAM_SIZE);
    ram_init_bus_device(ram, ram_bdev);

    tty_init(tty);
    tty_init_bus_device(tty, tty_bdev);

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

    for (const auto& f : std::filesystem::directory_iterator(argv[1])) {
        std::string path = f.path().string();
        std::string name = f.path().filename().string();

        printf("Running %s: ", name.c_str());

        int result = run_test(path);

        printf("%s\n", result ? "pass" : "fail");
    }

    ram_destroy(ram);
    bus_destroy(bus);
    rv32_destroy(cpu);
}