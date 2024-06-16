#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rv32.h"

struct rv32_state* rv32_create(void) {
    return malloc(sizeof(struct rv32_state));
}

void rv32_init(struct rv32_state* cpu, struct rv32_bus bus) {
    memset(cpu, 0, sizeof(struct rv32_state));

    cpu->bus = bus;
}

int rv32_execute(struct rv32_state* cpu, uint32_t opcode) {
    switch (opcode & 0xfff0707f) {
        case 0x00000033: rv32_i_add(cpu); return 1;
        case 0x02000033: rv32_i_sub(cpu); return 1;
        case 0x00004033: rv32_i_xor(cpu); return 1;
        case 0x00006033: rv32_i_or(cpu); return 1;
        case 0x00007033: rv32_i_and(cpu); return 1;
        case 0x00001033: rv32_i_sll(cpu); return 1;
        case 0x00005033: rv32_i_srl(cpu); return 1;
        case 0x02005033: rv32_i_sra(cpu); return 1;
        case 0x00002033: rv32_i_slt(cpu); return 1;
        case 0x00003033: rv32_i_sltu(cpu); return 1;
        case 0x00001013: rv32_i_slli(cpu); return 1;
        case 0x00005013: rv32_i_srli(cpu); return 1;
        case 0x02005013: rv32_i_srai(cpu); return 1;
        case 0x00000073: rv32_i_ecall(cpu); return 1;
        case 0x00100073: rv32_i_ebreak(cpu); return 1;
        case 0x00100033: rv32_i_mul(cpu); return 1;
        case 0x00101033: rv32_i_mulh(cpu); return 1;
        case 0x00102033: rv32_i_mulsu(cpu); return 1;
        case 0x00103033: rv32_i_mulu(cpu); return 1;
        case 0x00104033: rv32_i_div(cpu); return 1;
        case 0x00105033: rv32_i_divu(cpu); return 1;
        case 0x00106033: rv32_i_rem(cpu); return 1;
        case 0x00107033: rv32_i_remu(cpu); return 1;
        case 0x0000100f: rv32_i_fencei(cpu); return 1;
    }

    switch (opcode & 0x0000707f) {
        case 0x00000013: rv32_i_addi(cpu); return 1;
        case 0x00004013: rv32_i_xori(cpu); return 1;
        case 0x00006013: rv32_i_ori(cpu); return 1;
        case 0x00007013: rv32_i_andi(cpu); return 1;
        case 0x00002013: rv32_i_slti(cpu); return 1;
        case 0x00003013: rv32_i_sltiu(cpu); return 1;
        case 0x00000003: rv32_i_lb(cpu); return 1;
        case 0x00001003: rv32_i_lh(cpu); return 1;
        case 0x00002003: rv32_i_lw(cpu); return 1;
        case 0x00004003: rv32_i_lbu(cpu); return 1;
        case 0x00005003: rv32_i_lhu(cpu); return 1;
        case 0x00000023: rv32_i_sb(cpu); return 1;
        case 0x00001023: rv32_i_sh(cpu); return 1;
        case 0x00002023: rv32_i_sw(cpu); return 1;
        case 0x00000063: rv32_i_beq(cpu); return 1;
        case 0x00001063: rv32_i_bne(cpu); return 1;
        case 0x00004063: rv32_i_blt(cpu); return 1;
        case 0x00005063: rv32_i_bge(cpu); return 1;
        case 0x00006063: rv32_i_bltu(cpu); return 1;
        case 0x00007063: rv32_i_bgeu(cpu); return 1;
        case 0x00000067: rv32_i_jalr(cpu); return 1;
        case 0x00001073: rv32_i_csrrw(cpu); return 1;
        case 0x00002073: rv32_i_csrrs(cpu); return 1;
        case 0x00003073: rv32_i_csrrc(cpu); return 1;
        case 0x00005073: rv32_i_csrrwi(cpu); return 1;
        case 0x00006073: rv32_i_csrrsi(cpu); return 1;
        case 0x00007073: rv32_i_csrrci(cpu); return 1;
    }

    switch (opcode & 0x0000007f) {
        case 0x0000006f: rv32_i_jal(cpu); return 1;
        case 0x00000037: rv32_i_lui(cpu); return 1;
        case 0x00000017: rv32_i_auipc(cpu); return 1;
    }

    switch (opcode & 0xf800707f) {
        case 0x0000202f: rv32_i_amoadd(cpu); return 1;
        case 0x0800202f: rv32_i_amoswap(cpu); return 1;
        case 0x1000202f: rv32_i_lr(cpu); return 1;
        case 0x1800202f: rv32_i_sc(cpu); return 1;
        case 0x2000202f: rv32_i_amoxor(cpu); return 1;
        case 0x5000202f: rv32_i_amoor(cpu); return 1;
        case 0x6000202f: rv32_i_amoand(cpu); return 1;
        case 0x8000202f: rv32_i_amomin(cpu); return 1;
        case 0xa000202f: rv32_i_amomax(cpu); return 1;
        case 0xc000202f: rv32_i_amominu(cpu); return 1;
        case 0xe000202f: rv32_i_amomaxu(cpu); return 1;
    }

    return 0;
}

void rv32_cycle(struct rv32_state* cpu) {
    uint32_t opcode = rv32_bus_read32(cpu, cpu->pc);

    if (!rv32_execute(cpu, opcode)) {
        printf("Unimplemented instruction %08x\n", opcode);

        exit(1);
    }

    cpu->pc += 4;
}

uint32_t rv32_bus_read32(struct rv32_state* cpu, uint32_t addr) {
    return cpu->bus.read32(cpu->bus.udata, addr);
}

uint32_t rv32_bus_read16(struct rv32_state* cpu, uint32_t addr) {
    return cpu->bus.read16(cpu->bus.udata, addr);
}

uint32_t rv32_bus_read8(struct rv32_state* cpu, uint32_t addr) {
    return cpu->bus.read8(cpu->bus.udata, addr);
}

void rv32_bus_write32(struct rv32_state* cpu, uint32_t addr, uint32_t data) {
    cpu->bus.write32(cpu->bus.udata, addr, data);
}

void rv32_bus_write16(struct rv32_state* cpu, uint32_t addr, uint32_t data) {
    cpu->bus.write16(cpu->bus.udata, addr, data);
}

void rv32_bus_write8(struct rv32_state* cpu, uint32_t addr, uint32_t data) {
    cpu->bus.write8(cpu->bus.udata, addr, data);
}

void rv32_destroy(struct rv32_state* cpu) {
    free(cpu);
}

void rv32_set_pc(struct rv32_state* cpu, uint32_t pc) {
    cpu->pc = pc;
}