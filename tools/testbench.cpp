#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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

void get_skipped_tests(std::vector <std::string>& v, std::string path) {
    std::ifstream file(path);

    while (!file.eof()) {
        std::string s;

        file >> s;

        v.push_back(s);
    }

    file.close();
}

int run_test(std::string path) {
    elf_file_t* elf = elf_create();

    if (elf_load(elf, path.c_str()))
        return 1;

    // puts("Program Headers:\n");
    // puts("  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align");

    for (int i = 0; i < elf->ehdr->e_phnum; i++) {
        Elf32_Phdr* phdr = elf->phdr[i];

        if (elf->phdr[i]->p_type != PT_LOAD)
            continue;

        // printf("  %-14s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %c%c%c 0x%x\n",
        //     "LOAD",
        //     phdr->p_offset,
        //     phdr->p_vaddr,
        //     phdr->p_paddr,
        //     phdr->p_filesz,
        //     phdr->p_memsz,
        //     phdr->p_flags & PF_R ? 'R' : ' ',
        //     phdr->p_flags & PF_W ? 'W' : ' ',
        //     phdr->p_flags & PF_X ? 'X' : ' ',
        //     phdr->p_align
        // );

        elf_load_segment(elf, i, &ram->buf[phdr->p_paddr - RAM_START]);
    }

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

    std::vector <std::string> skip;

    for (const auto& f : std::filesystem::directory_iterator(argv[1])) {
        std::string path = f.path().string();
        std::string name = f.path().filename().string();

        if (name == ".skip")
            get_skipped_tests(skip, path);
    }

    int pass = 0;
    int fail = 0;
    int skipped = 0;

    for (const auto& f : std::filesystem::directory_iterator(argv[1])) {
        std::string path = f.path().string();
        std::string name = f.path().filename().string();

        // Skip dump files
        if (f.path().extension() == ".dump")
            continue;

        if (name == ".skip")
            continue;

        printf("Running %-25s ", name.c_str());

        if (std::find(skip.begin(), skip.end(), name) != skip.end()) {
            printf("\x1b[33mskipped\x1b[0m\n");

            ++skipped;

            continue;
        }

        int result = run_test(path);

        printf("%s\n", result ? "\x1b[32mpass\x1b[0m" : "\x1b[31mfail\x1b[0m");

        if (result) {
            ++pass;
        } else {
            ++fail;
        }
    }

    if (!fail) {
        printf("Passed all tests! (%u out of %u skipped)\n", skipped, skipped + pass);
    } else {
        printf("Passed %u tests, failed %u tests (%u out of %u skipped) success ratio: %.1f%%",
            pass, fail, skipped, skipped + pass + fail,
            (1.0f / (((float)pass + (float)fail) / (float)pass)) * 100.0f
        );
    }

    ram_destroy(ram);
    bus_destroy(bus);
    rv32_destroy(cpu);
}