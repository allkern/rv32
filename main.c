#include <stdio.h>

#include "rv32/rv32.h"
#include "machine/bus.h"
#include "machine/ram.h"
#include "machine/tty.h"
#include "elf_ldr.h"

#define RAM_START 0x80000000
#define TTY_START 0xa0000000
#define RAM_SIZE 0x1000000
#define TTY_SIZE 0x8

int main(int argc, const char* argv[]) {
    struct rv32_state* cpu = rv32_create();
    struct bus_state* bus = bus_create();
    struct ram_state* ram = ram_create();
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

    // FILE* file = fopen("test.bin", "rb");

    // fread(ram->buf, 1, RAM_SIZE, file);
    // fclose(file);

    elf_file_t* elf = elf_create();

    if (elf_load(elf, argv[1]))
        return 1;

    puts("Program Headers:");
    puts("  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align");

    for (int i = 0; i < elf->ehdr->e_phnum; i++) {
        Elf32_Phdr* phdr = elf->phdr[i];

        printf("  %-14s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %c%c%c 0x%x\n",
            "SEGMENT",
            phdr->p_offset,
            phdr->p_vaddr,
            phdr->p_paddr,
            phdr->p_filesz,
            phdr->p_memsz,
            phdr->p_flags & PF_R ? 'R' : ' ',
            phdr->p_flags & PF_W ? 'W' : ' ',
            phdr->p_flags & PF_X ? 'X' : ' ',
            phdr->p_align
        );

        if (elf->phdr[i]->p_type != PT_LOAD)
            continue;

        elf_load_segment(elf, i, &ram->buf[phdr->p_paddr - RAM_START]);
    }

    // Initialize stack
    cpu->x[2] = RAM_START + RAM_SIZE;

    // Set PC to entry point
    rv32_set_pc(cpu, elf->ehdr->e_entry);

    printf("Initialized CPU with PC=%08x SP=%08x\n",
        cpu->pc,
        cpu->x[2]
    );

    while (1)
        rv32_cycle(cpu);

    ram_destroy(ram);
    bus_destroy(bus);
    rv32_destroy(cpu);
}