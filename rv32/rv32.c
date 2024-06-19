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
    switch (opcode & 0xfe00707f) {
        case 0x00000033: rv32_i_add(cpu); return 1;
        case 0x40000033: rv32_i_sub(cpu); return 1;
        case 0x00004033: rv32_i_xor(cpu); return 1;
        case 0x00006033: rv32_i_or(cpu); return 1;
        case 0x00007033: rv32_i_and(cpu); return 1;
        case 0x00001033: rv32_i_sll(cpu); return 1;
        case 0x00005033: rv32_i_srl(cpu); return 1;
        case 0x40005033: rv32_i_sra(cpu); return 1;
        case 0x00002033: rv32_i_slt(cpu); return 1;
        case 0x00003033: rv32_i_sltu(cpu); return 1;
        case 0x00001013: rv32_i_slli(cpu); return 1;
        case 0x00005013: rv32_i_srli(cpu); return 1;
        case 0x40005013: rv32_i_srai(cpu); return 1;
        case 0x00100033: rv32_i_mul(cpu); return 1;
        case 0x00101033: rv32_i_mulh(cpu); return 1;
        case 0x00102033: rv32_i_mulhsu(cpu); return 1;
        case 0x00103033: rv32_i_mulhu(cpu); return 1;
        case 0x00104033: rv32_i_div(cpu); return 1;
        case 0x00105033: rv32_i_divu(cpu); return 1;
        case 0x00106033: rv32_i_rem(cpu); return 1;
        case 0x00107033: rv32_i_remu(cpu); return 1;
        case 0x20000053: rv32_i_fsgnjs(cpu); return 1;
        case 0x20001053: rv32_i_fsgnjns(cpu); return 1;
        case 0x20002053: rv32_i_fsgnjxs(cpu); return 1;
        case 0x28000053: rv32_i_fmins(cpu); return 1;
        case 0x28001053: rv32_i_fmaxs(cpu); return 1;
        case 0xa0000053: rv32_i_fles(cpu); return 1;
        case 0xa0001053: rv32_i_flts(cpu); return 1;
        case 0xa0002053: rv32_i_feqs(cpu); return 1;
        case 0x22000053: rv32_i_fsgnjd(cpu); return 1;
        case 0x22001053: rv32_i_fsgnjnd(cpu); return 1;
        case 0x22002053: rv32_i_fsgnjxd(cpu); return 1;
        case 0x2a000053: rv32_i_fmind(cpu); return 1;
        case 0x2a001053: rv32_i_fmaxd(cpu); return 1;
        case 0xa2000053: rv32_i_fled(cpu); return 1;
        case 0xa2001053: rv32_i_fltd(cpu); return 1;
        case 0xa2002053: rv32_i_feqd(cpu); return 1;
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
        case 0x0000000f: rv32_i_fence(cpu); return 1;
        case 0x0000100f: rv32_i_fencei(cpu); return 1;
        case 0x00001073: rv32_i_csrrw(cpu); return 1;
        case 0x00002073: rv32_i_csrrs(cpu); return 1;
        case 0x00003073: rv32_i_csrrc(cpu); return 1;
        case 0x00005073: rv32_i_csrrwi(cpu); return 1;
        case 0x00006073: rv32_i_csrrsi(cpu); return 1;
        case 0x00007073: rv32_i_csrrci(cpu); return 1;
        case 0x00002007: rv32_i_flw(cpu); return 1;
        case 0x00002027: rv32_i_fsw(cpu); return 1;
        case 0x00003007: rv32_i_fld(cpu); return 1;
        case 0x00003027: rv32_i_fsd(cpu); return 1;
    }

    switch (opcode & 0x0000007f) {
        case 0x0000006f: rv32_i_jal(cpu); return 1;
        case 0x00000037: rv32_i_lui(cpu); return 1;
        case 0x00000017: rv32_i_auipc(cpu); return 1;
    }

    switch (opcode & 0xffffffff) {
        case 0x00000073: rv32_i_ecall(cpu); return 1;
        case 0x00100073: rv32_i_ebreak(cpu); return 1;
        case 0x10200073: rv32_i_sret(cpu); return 1;
        case 0x30200073: rv32_i_mret(cpu); return 1;
        case 0x70200073: rv32_i_mnret(cpu); return 1;
        case 0x10500073: rv32_i_wfi(cpu); return 1;
    }

    switch (opcode & 0xf800707f) {
        case 0x1000202f: rv32_i_lr(cpu); return 1;
        case 0x0000202f: rv32_i_amoadd(cpu); return 1;
        case 0x0800202f: rv32_i_amoswap(cpu); return 1;
        case 0x1800202f: rv32_i_sc(cpu); return 1;
        case 0x2000202f: rv32_i_amoxor(cpu); return 1;
        case 0x5000202f: rv32_i_amoor(cpu); return 1;
        case 0x6000202f: rv32_i_amoand(cpu); return 1;
        case 0x8000202f: rv32_i_amomin(cpu); return 1;
        case 0xa000202f: rv32_i_amomax(cpu); return 1;
        case 0xc000202f: rv32_i_amominu(cpu); return 1;
        case 0xe000202f: rv32_i_amomaxu(cpu); return 1;
    }

    switch (opcode & 0xfff0707f) {
        case 0xe0000053: rv32_i_fmvxw(cpu); return 1;
        case 0xe0001053: rv32_i_fclasss(cpu); return 1;
        case 0xf0000053: rv32_i_fmvwx(cpu); return 1;
        case 0xe2001053: rv32_i_fclassd(cpu); return 1;
    }

    switch (opcode & 0x0600007f) {
        case 0x00000043: rv32_i_fmadds(cpu); return 1;
        case 0x00000047: rv32_i_fmsubs(cpu); return 1;
        case 0x0000004b: rv32_i_fnmsubs(cpu); return 1;
        case 0x0000004f: rv32_i_fnmadds(cpu); return 1;
        case 0x02000043: rv32_i_fmaddd(cpu); return 1;
        case 0x02000047: rv32_i_fmsubd(cpu); return 1;
        case 0x0200004b: rv32_i_fnmsubd(cpu); return 1;
        case 0x0200004f: rv32_i_fnmaddd(cpu); return 1;
    }

    switch (opcode & 0xfe00007f) {
        case 0x00000053: rv32_i_fadds(cpu); return 1;
        case 0x08000053: rv32_i_fsubs(cpu); return 1;
        case 0x10000053: rv32_i_fmuls(cpu); return 1;
        case 0x18000053: rv32_i_fdivs(cpu); return 1;
        case 0x02000053: rv32_i_faddd(cpu); return 1;
        case 0x0a000053: rv32_i_fsubd(cpu); return 1;
        case 0x12000053: rv32_i_fmuld(cpu); return 1;
        case 0x1a000053: rv32_i_fdivd(cpu); return 1;
    }

    switch (opcode & 0xfff0007f) {
        case 0x58000053: rv32_i_fsqrts(cpu); return 1;
        case 0xc0000053: rv32_i_fcvtws(cpu); return 1;
        case 0xc0100053: rv32_i_fcvtwus(cpu); return 1;
        case 0xd0000053: rv32_i_fcvtsw(cpu); return 1;
        case 0xd0100053: rv32_i_fcvtswu(cpu); return 1;
        case 0x5a000053: rv32_i_fsqrtd(cpu); return 1;
        case 0x40100053: rv32_i_fcvtsd(cpu); return 1;
        case 0x42000053: rv32_i_fcvtds(cpu); return 1;
        case 0xc2000053: rv32_i_fcvtwd(cpu); return 1;
        case 0xc2100053: rv32_i_fcvtwud(cpu); return 1;
        case 0xd2000053: rv32_i_fcvtdw(cpu); return 1;
        case 0xd2100053: rv32_i_fcvtdwu(cpu); return 1;
    }

    return 0;
}

void rv32_cycle(struct rv32_state* cpu) {
    cpu->opcode = rv32_bus_read32(cpu, cpu->pc);

    cpu->x[0] = 0;

    if (!rv32_execute(cpu, cpu->opcode)) {
        printf("Unimplemented instruction %08x\n", cpu->opcode);

        exit(1);
    }

    cpu->x[0] = 0;
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