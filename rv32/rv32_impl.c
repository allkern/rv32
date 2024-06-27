#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fenv.h>

#include "rv32.h"

#define RV32_U_IMM ((cpu->opcode >> 12) & 0xfffff)
#define RV32_U_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_I_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_I_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_I_IMM ((cpu->opcode >> 20) & 0xfff)
#define RV32_S_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_S_RS2 ((cpu->opcode >> 20) & 0x1f)
#define RV32_S_IMM (((cpu->opcode >> 7 ) & 0x1f) | \
                   (((cpu->opcode >> 25) & 0x7f) << 5))
#define RV32_J_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_J_IMM ((((cpu->opcode >> 21) & 0x3ff) | \
                    (((cpu->opcode >> 20) & 1) << 10) | \
                    (((cpu->opcode >> 12) & 0xff) << 11) | \
                    (((cpu->opcode >> 31) & 1) << 19)) << 1)
#define RV32_B_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_B_RS2 ((cpu->opcode >> 20) & 0x1f)
#define RV32_B_IMM ((((cpu->opcode >> 8) & 0xf) | \
                    (((cpu->opcode >> 25) & 0x3f) << 4) | \
                    (((cpu->opcode >> 7) & 1) << 10) | \
                    (((cpu->opcode >> 31) & 1) << 11)) << 1)
#define RV32_CSR_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_CSR_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_CSR_CSR ((cpu->opcode >> 20) & 0xfff)
#define RV32_R_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_R_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_R_RS2 ((cpu->opcode >> 20) & 0x1f)
#define RV32_CIW_RD (((cpu->opcode >> 2) & 7) + 8)
#define RV32_CIW_IMM ((((cpu->opcode >> 6) & 1) | \
                      (((cpu->opcode >> 5) & 1) << 1) | \
                      (((cpu->opcode >> 11) & 3) << 2) | \
                      (((cpu->opcode >> 7) & 0xf) << 4)) << 2)
#define RV32_CI_IMMV ((((cpu->opcode >> 6) & 1) | \
                      (((cpu->opcode >> 2) & 1) << 1) | \
                      (((cpu->opcode >> 5) & 1) << 2) | \
                      (((cpu->opcode >> 3) & 3) << 3) | \
                      (((cpu->opcode >> 12) & 1) << 5)) << 4)
#define RV32_CI_IMMS ((((cpu->opcode >> 4) & 7) | \
                      (((cpu->opcode >> 12) & 1) << 3) | \
                      (((cpu->opcode >> 2) & 3) << 4)) << 2)
#define RV32_CI_IMM (((cpu->opcode >> 2) & 0x1f) | \
                     ((cpu->opcode >> 12) & 1) << 5)
#define RV32_CI_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_CL_IMM ((((cpu->opcode >> 6) & 1) | \
                     (((cpu->opcode >> 10) & 7) << 1) | \
                     (((cpu->opcode >> 5) & 1) << 4)) << 2)
#define RV32_CL_RD (((cpu->opcode >> 2) & 7) + 8)
#define RV32_CL_RS1 (((cpu->opcode >> 7) & 7) + 8)
#define RV32_CS_IMM ((((cpu->opcode >> 6) & 1) | \
                     (((cpu->opcode >> 10) & 7) << 1) | \
                     (((cpu->opcode >> 5) & 1) << 4)) << 2)
#define RV32_CS_RS2 (((cpu->opcode >> 2) & 7) + 8)
#define RV32_CS_RS1 (((cpu->opcode >> 7) & 7) + 8)
#define RV32_CB_IMMV ((((cpu->opcode >> 3) & 3) | \
                      (((cpu->opcode >> 10) & 3) << 2) | \
                      (((cpu->opcode >> 2) & 1) << 4) | \
                      (((cpu->opcode >> 5) & 3) << 5) | \
                      (((cpu->opcode >> 12) & 1) << 7)) << 1)
#define RV32_CB_IMM (((cpu->opcode >> 2) & 0x1f) | \
                    (((cpu->opcode >> 12) & 1) << 5))
#define RV32_CB_RD (((cpu->opcode >> 7) & 7) + 8)
#define RV32_CA_RD (((cpu->opcode >> 7) & 7) + 8)
#define RV32_CA_RS2 (((cpu->opcode >> 2) & 7) + 8)
#define RV32_CJ_IMM ((((cpu->opcode >> 3) & 7) | \
                     (((cpu->opcode >> 11) & 1) << 3) | \
                     (((cpu->opcode >> 2) & 1) << 4) | \
                     (((cpu->opcode >> 7) & 1) << 5) | \
                     (((cpu->opcode >> 6) & 1) << 6) | \
                     (((cpu->opcode >> 9) & 3) << 7) | \
                     (((cpu->opcode >> 8) & 1) << 9) | \
                     (((cpu->opcode >> 12) & 1) << 10)) << 1)
#define RV32_CR_RS2 ((cpu->opcode >> 2) & 0x1f)
#define RV32_CR_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_CSS_IMM ((((cpu->opcode >> 9) & 0xf) | \
                      (((cpu->opcode >> 7) & 3) << 4)) << 2)
#define RV32_CSS_RS2 ((cpu->opcode >> 2) & 0x1f)

#define SE32(v, b) (((int32_t)((v) << (32 - b))) >> (32 - b))
#define S64(v) ((int64_t)((int32_t)v))

#define MAXU(a, b) ((a) > (b) ? (a) : (b))
#define MINU(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) (((int32_t)a) > ((int32_t)b) ? (a) : (b))
#define MIN(a, b) (((int32_t)a) < ((int32_t)b) ? (a) : (b))

void rv32_i_add(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "add", d, s1, s2);

    cpu->x[d] = cpu->x[s1] + cpu->x[s2];
}
void rv32_i_sub(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sub", d, s1, s2);

    cpu->x[d] = cpu->x[s1] - cpu->x[s2];
}
void rv32_i_xor(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "xor", d, s1, s2);

    cpu->x[d] = cpu->x[s1] ^ cpu->x[s2];
}
void rv32_i_or(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "or", d, s1, s2);

    cpu->x[d] = cpu->x[s1] | cpu->x[s2];
}
void rv32_i_and(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "and", d, s1, s2);

    cpu->x[d] = cpu->x[s1] & cpu->x[s2];
}
void rv32_i_sll(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sll", d, s1, s2);

    cpu->x[d] = cpu->x[s1] << (cpu->x[s2] & 0x1f);
}
void rv32_i_srl(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "srl", d, s1, s2);

    cpu->x[d] = cpu->x[s1] >> (cpu->x[s2] & 0x1f);
}
void rv32_i_sra(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sra", d, s1, s2);

    cpu->x[d] = ((int32_t)cpu->x[s1]) >> (cpu->x[s2] & 0x1f);
}
void rv32_i_slt(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "slt", d, s1, s2);

    cpu->x[d] = (int32_t)cpu->x[s1] < (int32_t)cpu->x[s2];
}
void rv32_i_sltu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sltu", d, s1, s2);

    cpu->x[d] = cpu->x[s1] < cpu->x[s2];
}
void rv32_i_addi(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "addi", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] + SE32(imm, 12);
}
void rv32_i_xori(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "xori", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] ^ SE32(imm, 12);
}
void rv32_i_ori(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "ori", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] | SE32(imm, 12);
}
void rv32_i_andi(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "andi", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] & SE32(imm, 12);
}
void rv32_i_slli(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t shamt = RV32_I_IMM & 0x3f;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "slli", d, s1, shamt);

    cpu->x[d] = cpu->x[s1] << shamt;
}
void rv32_i_srli(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t shamt = RV32_I_IMM & 0x3f;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "srli", d, s1, shamt);

    cpu->x[d] = cpu->x[s1] >> shamt;
}
void rv32_i_srai(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t shamt = RV32_I_IMM & 0x3f;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "srai", d, s1, shamt);

    cpu->x[d] = ((int32_t)cpu->x[s1]) >> shamt;
}
void rv32_i_slti(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "slti", d, s1, SE32(imm, 12));

    cpu->x[d] = (int32_t)cpu->x[s1] < SE32(imm, 12);
}
void rv32_i_sltiu(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "sltiu", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] < (uint32_t)SE32(imm, 12);
}
void rv32_i_lb(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lb", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read8(cpu, cpu->x[s1] + SE32(imm, 12)) & 0xff;
    cpu->x[d] = SE32(cpu->x[d], 8);
}
void rv32_i_lh(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lh", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read16(cpu, cpu->x[s1] + SE32(imm, 12)) & 0xffff;
    cpu->x[d] = SE32(cpu->x[d], 16);
}
void rv32_i_lw(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lw", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1] + SE32(imm, 12));
}
void rv32_i_lbu(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lbu", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read8(cpu, cpu->x[s1] + SE32(imm, 12)) & 0xff;
}
void rv32_i_lhu(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lhu", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read16(cpu, cpu->x[s1] + SE32(imm, 12));
}
void rv32_i_sb(struct rv32_state* cpu) {
    uint32_t s1 = RV32_S_RS1;
    uint32_t s2 = RV32_S_RS2;
    uint32_t imm = RV32_S_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "sb", s2, SE32(imm, 12), s1);

    rv32_bus_write8(cpu, cpu->x[s1] + SE32(imm, 12), cpu->x[s2] & 0xff);
}
void rv32_i_sh(struct rv32_state* cpu) {
    uint32_t s1 = RV32_S_RS1;
    uint32_t s2 = RV32_S_RS2;
    uint32_t imm = RV32_S_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "sh", s2, SE32(imm, 12), s1);

    rv32_bus_write16(cpu, cpu->x[s1] + SE32(imm, 12), cpu->x[s2]);
}
void rv32_i_sw(struct rv32_state* cpu) {
    uint32_t s1 = RV32_S_RS1;
    uint32_t s2 = RV32_S_RS2;
    uint32_t imm = RV32_S_IMM;

    // printf("%08x: %08x %-10s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "sw", s2, SE32(imm, 12), s1);

    rv32_bus_write32(cpu, cpu->x[s1] + SE32(imm, 12), cpu->x[s2]);
}
void rv32_i_beq(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "beq", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] == cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bne(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "bne", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] != cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_blt(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "blt", s1, s2, cpu->pc + SE32(imm, 13));

    if ((int32_t)cpu->x[s1] < (int32_t)cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bge(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "bge", s1, s2, cpu->pc + SE32(imm, 13));

    if ((int32_t)cpu->x[s1] >= (int32_t)cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bltu(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "bltu", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] < cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bgeu(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "bgeu", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] >= cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_jal(struct rv32_state* cpu) {
    uint32_t d = RV32_J_RD;
    uint32_t imm = RV32_J_IMM;

    // printf("%08x: %08x %-10s x%u, 0x%x\n", cpu->pc, cpu->opcode, "jal", d, cpu->pc + SE32(imm, 21));

    cpu->x[d] = cpu->pc + 4;
    cpu->pc = (cpu->pc - 4) + SE32(imm, 21);
}
void rv32_i_jalr(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s x%u, 0x%x\n", cpu->pc, cpu->opcode, "jalr", d, (cpu->x[s1] + SE32(imm, 12)) & 0xfffffffe);

    uint32_t target = ((cpu->x[s1] + SE32(imm, 12)) & 0xfffffffe) - 4;

    cpu->x[d] = cpu->pc + 4;
    cpu->pc = target;
}
void rv32_i_lui(struct rv32_state* cpu) {
    uint32_t d = RV32_U_RD;
    uint32_t imm = RV32_U_IMM;

    // printf("%08x: %08x %-10s x%u, 0x%x\n", cpu->pc, cpu->opcode, "lui", d, imm);

    cpu->x[d] = imm << 12;
}
void rv32_i_auipc(struct rv32_state* cpu) {
    uint32_t d = RV32_U_RD;
    uint32_t imm = RV32_U_IMM;

    // printf("%08x: %08x %-10s x%u, 0x%x\n", cpu->pc, cpu->opcode, "auipc", d, cpu->pc + (imm << 12));

    cpu->x[d] = cpu->pc + (imm << 12);
}
void rv32_i_ecall(struct rv32_state* cpu) {
    // printf("%08x: %08x %s\n", cpu->pc, cpu->opcode, "ecall");

    if (cpu->x[17] != 93)
        return;

    exit(1);
}
void rv32_i_ebreak(struct rv32_state* cpu) { puts("rv32: ebreak unimplemented"); exit(1); }
void rv32_i_mul(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "mul", d, s1, s2);

    cpu->x[d] = cpu->x[s1] * cpu->x[s2];
}
void rv32_i_mulh(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    uint64_t r = S64(cpu->x[s1]) * S64(cpu->x[s2]);

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "mulh", d, s1, s2);

    cpu->x[d] = r >> 32;
}
void rv32_i_mulhsu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    uint64_t r = S64(cpu->x[s1]) * cpu->x[s2];

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "mulhsu", d, s1, s2);

    cpu->x[d] = r >> 32;
}
void rv32_i_mulhu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    uint64_t r = (uint64_t)cpu->x[s1] * (uint64_t)cpu->x[s2];

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "mulhu", d, s1, s2);

    cpu->x[d] = r >> 32;
}
void rv32_i_div(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "div", d, s1, s2);

    if (!cpu->x[s2]) {
        cpu->x[d] = -1;

        return;
    }

    if (cpu->x[s1] == 0x80000000 && cpu->x[s2] == 0xffffffff) {
        cpu->x[d] = 0x80000000;

        return;
    }

    cpu->x[d] = (int32_t)cpu->x[s1] / (int32_t)cpu->x[s2];
}
void rv32_i_divu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "divu", d, s1, s2);

    if (!cpu->x[s2]) {
        cpu->x[d] = 0xffffffff;

        return;
    }

    cpu->x[d] = cpu->x[s1] / cpu->x[s2];
}
void rv32_i_rem(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "rem", d, s1, s2);

    if (!cpu->x[s2]) {
        cpu->x[d] = cpu->x[s1];

        return;
    }

    if (cpu->x[s1] == 0x80000000 && cpu->x[s2] == 0xffffffff) {
        cpu->x[d] = 0;

        return;
    }

    cpu->x[d] = (int32_t)cpu->x[s1] % (int32_t)cpu->x[s2];
}
void rv32_i_remu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "remu", d, s1, s2);

    if (!cpu->x[s2]) {
        cpu->x[d] = cpu->x[s1];

        return;
    }

    cpu->x[d] = cpu->x[s1] % cpu->x[s2];
}
void rv32_i_amoadd(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amoadd.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[d] + cpu->x[s2]);
}
void rv32_i_amoswap(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amoswap.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[s2]);
}
void rv32_i_lr(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "lr.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    cpu->res_addr = cpu->x[s1];
    cpu->res_value = cpu->x[d];
    cpu->res_valid = 1;
}
void rv32_i_sc(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sc.w", d, s1, s2);

    if (cpu->x[s1] != cpu->res_addr || !cpu->res_valid) {
        cpu->x[d] = 1;

        return;
    }

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[s2]);

    cpu->res_valid = 0;
    cpu->x[d] = 0;
}
void rv32_i_amoxor(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amoxor.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[d] ^ cpu->x[s2]);
}
void rv32_i_amoor(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amoor.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[d] | cpu->x[s2]);
}
void rv32_i_amoand(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amoand.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], cpu->x[d] & cpu->x[s2]);
}
void rv32_i_amomin(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amomin.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], MIN(cpu->x[d], cpu->x[s2]));
}
void rv32_i_amomax(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amomax.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], MAX(cpu->x[d], cpu->x[s2]));
}
void rv32_i_amominu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amominu.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], MINU(cpu->x[d], cpu->x[s2]));
}
void rv32_i_amomaxu(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "amomaxu.w", d, s1, s2);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1]);

    rv32_bus_write32(cpu, cpu->x[s1], MAXU(cpu->x[d], cpu->x[s2]));
}
void rv32_i_fence(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;

    // NOP for now
    // printf("%08x: %08x %-10s x%u, x%u\n", cpu->pc, cpu->opcode, "fence", d, s1);
}
void rv32_i_fencei(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;

    // NOP for now
    // printf("%08x: %08x %-10s x%u, x%u\n", cpu->pc, cpu->opcode, "fence.i", d, s1);
}
void rv32_i_csrrw(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    if (csr == 1) {
        cpu->x[d] = cpu->fcsr & 0x1f;
    }

    // NOP for now
    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "csrrw", d, s1, csr);
}
void rv32_i_csrrs(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    // NOP for now
    // printf("%08x: %08x %-10s x%u, x%u, 0x%x\n", cpu->pc, cpu->opcode, "csrrs", d, s1, csr);
}
void rv32_i_csrrc(struct rv32_state* cpu) { puts("rv32: csrrc unimplemented"); exit(1); }
void rv32_i_csrrwi(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    // NOP for now
    // printf("%08x: %08x %-10s x%u, 0x%x, 0x%x\n", cpu->pc, cpu->opcode, "csrrwi", d, s1, csr);
}
void rv32_i_csrrsi(struct rv32_state* cpu) { puts("rv32: csrrsi unimplemented"); exit(1); }
void rv32_i_csrrci(struct rv32_state* cpu) { puts("rv32: csrrci unimplemented"); exit(1); }
void rv32_i_flw(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-10s f%u, %d(x%u)\n", cpu->pc, cpu->opcode, "flw", d, SE32(imm, 12), s1);

    cpu->f[d].u32 = rv32_bus_read32(cpu, cpu->x[s1] + SE32(imm, 12));
}
void rv32_i_fsw(struct rv32_state* cpu) { puts("rv32: fsw unimplemented"); exit(1); }
void rv32_i_fmadds(struct rv32_state* cpu) { puts("rv32: fmadds unimplemented"); exit(1); }
void rv32_i_fmsubs(struct rv32_state* cpu) { puts("rv32: fmsubs unimplemented"); exit(1); }
void rv32_i_fnmsubs(struct rv32_state* cpu) { puts("rv32: fnmsubs unimplemented"); exit(1); }
void rv32_i_fnmadds(struct rv32_state* cpu) { puts("rv32: fnmadds unimplemented"); exit(1); }
void rv32_i_fadds(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s f%u, f%u, f%u\n", cpu->pc, cpu->opcode, "fadd.s", d, s1, s2);

    feclearexcept(FE_ALL_EXCEPT);

    cpu->f[d].f = cpu->f[s1].f + cpu->f[s2].f;

    cpu->fcsr &= 0xffffffe0;

    if (fetestexcept(FE_INEXACT))   cpu->fcsr |= (1 << 0);
    if (fetestexcept(FE_UNDERFLOW)) cpu->fcsr |= (1 << 1);
    if (fetestexcept(FE_OVERFLOW))  cpu->fcsr |= (1 << 2);
    if (fetestexcept(FE_DIVBYZERO)) cpu->fcsr |= (1 << 3);
    if (fetestexcept(FE_INVALID))   cpu->fcsr |= (1 << 4);
}
void rv32_i_fsubs(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s f%u, f%u, f%u\n", cpu->pc, cpu->opcode, "fsub.s", d, s1, s2);

    feclearexcept(FE_ALL_EXCEPT);

    cpu->f[d].f = cpu->f[s1].f - cpu->f[s2].f;

    cpu->fcsr &= 0xffffffe0;

    if (fetestexcept(FE_INEXACT))   cpu->fcsr |= (1 << 0);
    if (fetestexcept(FE_UNDERFLOW)) cpu->fcsr |= (1 << 1);
    if (fetestexcept(FE_OVERFLOW))  cpu->fcsr |= (1 << 2);
    if (fetestexcept(FE_DIVBYZERO)) cpu->fcsr |= (1 << 3);
    if (fetestexcept(FE_INVALID))   cpu->fcsr |= (1 << 4);
}
void rv32_i_fmuls(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-10s f%u, f%u, f%u\n", cpu->pc, cpu->opcode, "fmul.s", d, s1, s2);

    feclearexcept(FE_ALL_EXCEPT);

    cpu->f[d].f = cpu->f[s1].f * cpu->f[s2].f;

    cpu->fcsr &= 0xffffffe0;

    if (fetestexcept(FE_INEXACT))   { cpu->fcsr |= (1 << 0); }
    if (fetestexcept(FE_UNDERFLOW)) { cpu->fcsr |= (1 << 1); }
    if (fetestexcept(FE_OVERFLOW))  { cpu->fcsr |= (1 << 2); }
    if (fetestexcept(FE_DIVBYZERO)) { cpu->fcsr |= (1 << 3); }
    if (fetestexcept(FE_INVALID))   { cpu->fcsr |= (1 << 4); }
}
void rv32_i_fdivs(struct rv32_state* cpu) { puts("rv32: fdivs unimplemented"); exit(1); }
void rv32_i_fsqrts(struct rv32_state* cpu) { puts("rv32: fsqrts unimplemented"); exit(1); }
void rv32_i_fsgnjs(struct rv32_state* cpu) { puts("rv32: fsgnjs unimplemented"); exit(1); }
void rv32_i_fsgnjns(struct rv32_state* cpu) { puts("rv32: fsgnjns unimplemented"); exit(1); }
void rv32_i_fsgnjxs(struct rv32_state* cpu) { puts("rv32: fsgnjxs unimplemented"); exit(1); }
void rv32_i_fmins(struct rv32_state* cpu) { puts("rv32: fmins unimplemented"); exit(1); }
void rv32_i_fmaxs(struct rv32_state* cpu) { puts("rv32: fmaxs unimplemented"); exit(1); }
void rv32_i_fcvtws(struct rv32_state* cpu) { puts("rv32: fcvtws unimplemented"); exit(1); }
void rv32_i_fcvtwus(struct rv32_state* cpu) { puts("rv32: fcvtwus unimplemented"); exit(1); }
void rv32_i_fmvxw(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;

    // printf("%08x: %08x %-10s x%u, f%u\n", cpu->pc, cpu->opcode, "fmv.x.w", d, s1);

    cpu->x[d] = cpu->f[s1].u32;
}
void rv32_i_fles(struct rv32_state* cpu) { puts("rv32: fles unimplemented"); exit(1); }
void rv32_i_flts(struct rv32_state* cpu) { puts("rv32: flts unimplemented"); exit(1); }
void rv32_i_feqs(struct rv32_state* cpu) { puts("rv32: feqs unimplemented"); exit(1); }
void rv32_i_fclasss(struct rv32_state* cpu) { puts("rv32: fclasss unimplemented"); exit(1); }
void rv32_i_fcvtsw(struct rv32_state* cpu) { puts("rv32: fcvtsw unimplemented"); exit(1); }
void rv32_i_fcvtswu(struct rv32_state* cpu) { puts("rv32: fcvtswu unimplemented"); exit(1); }
void rv32_i_fmvwx(struct rv32_state* cpu) { puts("rv32: fmvwx unimplemented"); exit(1); }
void rv32_i_fld(struct rv32_state* cpu) { puts("rv32: fld unimplemented"); exit(1); }
void rv32_i_fsd(struct rv32_state* cpu) { puts("rv32: fsd unimplemented"); exit(1); }
void rv32_i_fmaddd(struct rv32_state* cpu) { puts("rv32: fmaddd unimplemented"); exit(1); }
void rv32_i_fmsubd(struct rv32_state* cpu) { puts("rv32: fmsubd unimplemented"); exit(1); }
void rv32_i_fnmsubd(struct rv32_state* cpu) { puts("rv32: fnmsubd unimplemented"); exit(1); }
void rv32_i_fnmaddd(struct rv32_state* cpu) { puts("rv32: fnmaddd unimplemented"); exit(1); }
void rv32_i_faddd(struct rv32_state* cpu) { puts("rv32: faddd unimplemented"); exit(1); }
void rv32_i_fsubd(struct rv32_state* cpu) { puts("rv32: fsubd unimplemented"); exit(1); }
void rv32_i_fmuld(struct rv32_state* cpu) { puts("rv32: fmuld unimplemented"); exit(1); }
void rv32_i_fdivd(struct rv32_state* cpu) { puts("rv32: fdivd unimplemented"); exit(1); }
void rv32_i_fsqrtd(struct rv32_state* cpu) { puts("rv32: fsqrtd unimplemented"); exit(1); }
void rv32_i_fsgnjd(struct rv32_state* cpu) { puts("rv32: fsgnjd unimplemented"); exit(1); }
void rv32_i_fsgnjnd(struct rv32_state* cpu) { puts("rv32: fsgnjnd unimplemented"); exit(1); }
void rv32_i_fsgnjxd(struct rv32_state* cpu) { puts("rv32: fsgnjxd unimplemented"); exit(1); }
void rv32_i_fmind(struct rv32_state* cpu) { puts("rv32: fmind unimplemented"); exit(1); }
void rv32_i_fmaxd(struct rv32_state* cpu) { puts("rv32: fmaxd unimplemented"); exit(1); }
void rv32_i_fcvtsd(struct rv32_state* cpu) { puts("rv32: fcvtsd unimplemented"); exit(1); }
void rv32_i_fcvtds(struct rv32_state* cpu) { puts("rv32: fcvtds unimplemented"); exit(1); }
void rv32_i_fled(struct rv32_state* cpu) { puts("rv32: fled unimplemented"); exit(1); }
void rv32_i_fltd(struct rv32_state* cpu) { puts("rv32: fltd unimplemented"); exit(1); }
void rv32_i_feqd(struct rv32_state* cpu) { puts("rv32: feqd unimplemented"); exit(1); }
void rv32_i_fclassd(struct rv32_state* cpu) { puts("rv32: fclassd unimplemented"); exit(1); }
void rv32_i_fcvtwd(struct rv32_state* cpu) { puts("rv32: fcvtwd unimplemented"); exit(1); }
void rv32_i_fcvtwud(struct rv32_state* cpu) { puts("rv32: fcvtwud unimplemented"); exit(1); }
void rv32_i_fcvtdw(struct rv32_state* cpu) { puts("rv32: fcvtdw unimplemented"); exit(1); }
void rv32_i_fcvtdwu(struct rv32_state* cpu) { puts("rv32: fcvtdwu unimplemented"); exit(1); }
void rv32_i_sret(struct rv32_state* cpu) { puts("rv32: sret unimplemented"); exit(1); }
void rv32_i_mret(struct rv32_state* cpu) {
    // NOP for now
    // printf("%08x: %08x %-10s\n", cpu->pc, cpu->opcode, "mret");
}
void rv32_i_mnret(struct rv32_state* cpu) { puts("rv32: mnret unimplemented"); exit(1); }
void rv32_i_wfi(struct rv32_state* cpu) { puts("rv32: wfi unimplemented"); exit(1); }

void rv32_i_caddi4spn(struct rv32_state* cpu) {
    uint32_t d = RV32_CIW_RD;
    uint32_t imm = RV32_CIW_IMM; assert(imm != 0);

    // printf("%08x: %04x     %-10s x%u, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.addi4spn", d, imm);

    cpu->x[d] = cpu->x[2] + imm;
}
void rv32_i_cfld(struct rv32_state* cpu) { puts("rv32: cfld unimplemented"); exit(1); }
void rv32_i_clw(struct rv32_state* cpu) {
    uint32_t d = RV32_CL_RD;
    uint32_t s1 = RV32_CL_RS1;
    uint32_t imm = RV32_CL_IMM;

    // printf("%08x: %04x     %-10s x%u, %d(x%d)\n", cpu->pc, cpu->opcode & 0xffff, "c.lw", d, imm, s1);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1] + imm);

    // printf("  read %08x from %08x into x%d\n", cpu->x[d], cpu->x[s1] + imm, d);
}
void rv32_i_cflw(struct rv32_state* cpu) { puts("rv32: cflw unimplemented"); exit(1); }
void rv32_i_cfsd(struct rv32_state* cpu) { puts("rv32: cfsd unimplemented"); exit(1); }
void rv32_i_csw(struct rv32_state* cpu) {
    uint32_t s1 = RV32_CS_RS1;
    uint32_t s2 = RV32_CS_RS2;
    uint32_t imm = RV32_CS_IMM;

    // printf("%08x: %04x     %-10s x%u, %d(x%d)\n", cpu->pc, cpu->opcode & 0xffff, "c.sw", s2, imm, s1);

    rv32_bus_write32(cpu, cpu->x[s1] + imm, cpu->x[s2]);

    // printf("  wrote %08x to %08x\n", cpu->x[s2], cpu->x[s1] + imm);
}
void rv32_i_cfsw(struct rv32_state* cpu) { puts("rv32: cfsw unimplemented"); exit(1); }
void rv32_i_cnop(struct rv32_state* cpu) {
    // printf("%08x: %04x     %-10s\n", cpu->pc, cpu->opcode & 0xffff, "c.nop");
}
void rv32_i_caddi(struct rv32_state* cpu) {
    uint32_t imm = RV32_CI_IMM; assert(imm != 0);
    uint32_t d = RV32_CI_RD;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.addi", d, SE32(imm, 6));

    cpu->x[d] += SE32(imm, 6);
    
}
void rv32_i_cjal(struct rv32_state* cpu) {
    uint32_t imm = RV32_CJ_IMM;

    // printf("%08x: %04x     %-10s %08x\n", cpu->pc, cpu->opcode & 0xffff, "c.jal", cpu->pc + SE32(imm, 12));

    cpu->x[1] = cpu->pc + 2;
    cpu->pc += SE32(imm, 12);
    cpu->pc -= 2;
}
void rv32_i_cli(struct rv32_state* cpu) {
    uint32_t imm = RV32_CI_IMM;
    uint32_t d = RV32_CI_RD; assert(d != 0);

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.li", d, SE32(imm, 6));

    cpu->x[d] = SE32(imm, 6);
}
void rv32_i_caddi16sp(struct rv32_state* cpu) {
    uint32_t imm = RV32_CI_IMMV; assert(imm != 0);

    // printf("%08x: %04x     %-10s %d\n", cpu->pc, cpu->opcode & 0xffff, "c.add16sp", SE32(imm, 10));

    cpu->x[2] += SE32(imm, 10);
}
void rv32_i_clui(struct rv32_state* cpu) {
    uint32_t imm = RV32_CI_IMM; assert(imm != 0);
    uint32_t d = RV32_CI_RD; assert(d != 0);

    // printf("%08x: %04x     %-10s x%d, 0x%x\n", cpu->pc, cpu->opcode & 0xffff, "c.lui", d, SE32(imm << 12, 18));

    cpu->x[d] = SE32(imm << 12, 18);
}
void rv32_i_csrli(struct rv32_state* cpu) {
    uint32_t shamt = RV32_CB_IMM; assert(shamt < 16 && shamt);
    uint32_t d = RV32_CB_RD;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.srli", d, shamt);

    cpu->x[d] = cpu->x[d] >> shamt;
}
void rv32_i_csrai(struct rv32_state* cpu) {
    uint32_t shamt = RV32_CB_IMM; assert(shamt < 16 && shamt);
    uint32_t d = RV32_CB_RD;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.srai", d, shamt);

    cpu->x[d] = ((int32_t)cpu->x[d]) >> shamt;
}
void rv32_i_candi(struct rv32_state* cpu) {
    uint32_t imm = RV32_CB_IMM;
    uint32_t d = RV32_CB_RD;

    // printf("%08x: %04x     %-10s x%d, 0x%x\n", cpu->pc, cpu->opcode & 0xffff, "c.andi", d, SE32(imm, 6));

    cpu->x[d] &= SE32(imm, 6);
}
void rv32_i_csub(struct rv32_state* cpu) {
    uint32_t d = RV32_CA_RD;
    uint32_t s2 = RV32_CA_RS2;

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.sub", d, s2);

    cpu->x[d] -= cpu->x[s2];
}
void rv32_i_cxor(struct rv32_state* cpu) {
    uint32_t d = RV32_CA_RD;
    uint32_t s2 = RV32_CA_RS2;

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.xor", d, s2);

    cpu->x[d] ^= cpu->x[s2];
}
void rv32_i_cor(struct rv32_state* cpu) {
    uint32_t d = RV32_CA_RD;
    uint32_t s2 = RV32_CA_RS2;

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.or", d, s2);

    cpu->x[d] |= cpu->x[s2];
}
void rv32_i_cand(struct rv32_state* cpu) {
    uint32_t d = RV32_CA_RD;
    uint32_t s2 = RV32_CA_RS2;

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.and", d, s2);

    cpu->x[d] &= cpu->x[s2];
}
void rv32_i_cj(struct rv32_state* cpu) {
    uint32_t imm = RV32_CJ_IMM;

    // printf("%08x: %04x     %-10s %08x\n", cpu->pc, cpu->opcode & 0xffff, "c.j", cpu->pc + SE32(imm, 12));

    cpu->pc += SE32(imm, 12);
    cpu->pc -= 2;
}
void rv32_i_cbeqz(struct rv32_state* cpu) {
    uint32_t imm = RV32_CB_IMMV;
    uint32_t d = RV32_CB_RD;

    // printf("%08x: %04x     %-10s x%d, %08x\n", cpu->pc, cpu->opcode & 0xffff, "c.beqz", d, cpu->pc + SE32(imm, 9));

    if (!cpu->x[d]) {
        cpu->pc += SE32(imm, 9);
        cpu->pc -= 2;
    }
}
void rv32_i_cbnez(struct rv32_state* cpu) {
    uint32_t imm = RV32_CB_IMMV;
    uint32_t d = RV32_CB_RD;

    // printf("%08x: %04x     %-10s x%d, %08x\n", cpu->pc, cpu->opcode & 0xffff, "c.bnez", d, cpu->pc + SE32(imm, 9));

    if (cpu->x[d]) {
        cpu->pc += SE32(imm, 9);
        cpu->pc -= 2;
    }
}
void rv32_i_cslli(struct rv32_state* cpu) {
    uint32_t shamt = RV32_CI_IMM; assert(shamt < 16 && shamt);
    uint32_t d = RV32_CI_RD;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.slli", d, shamt);

    cpu->x[d] <<= shamt;
}
void rv32_i_cfldsp(struct rv32_state* cpu) { puts("rv32: cfldsp unimplemented"); exit(1); }
void rv32_i_clwsp(struct rv32_state* cpu) {
    uint32_t imm = RV32_CI_IMMS;
    uint32_t d = RV32_CI_RD;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.lwsp", d, imm);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[2] + imm);
}
void rv32_i_cflwsp(struct rv32_state* cpu) { puts("rv32: cflwsp unimplemented"); exit(1); }
void rv32_i_cjr(struct rv32_state* cpu) {
    uint32_t s1 = RV32_CR_RD; assert(s1 != 0);

    // printf("%08x: %04x     %-10s x%d (%08x)\n", cpu->pc, cpu->opcode & 0xffff, "c.jr", s1, cpu->x[s1]);

    cpu->pc = cpu->x[s1];
    cpu->pc -= 2;
}
void rv32_i_cmv(struct rv32_state* cpu) {
    uint32_t s2 = RV32_CR_RS2;
    uint32_t d = RV32_CR_RD; assert(d && s2);

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.mv", d, s2);

    cpu->x[d] = cpu->x[s2];
}
void rv32_i_cebreak(struct rv32_state* cpu) { puts("rv32: cebreak unimplemented"); exit(1); }
void rv32_i_cjalr(struct rv32_state* cpu) {
    uint32_t s1 = RV32_CR_RD; assert(s1 != 0);
    uint32_t rs1 = cpu->x[s1];

    // printf("%08x: %04x     %-10s x%d (%08x)\n", cpu->pc, cpu->opcode & 0xffff, "c.jalr", s1, cpu->x[s1]);

    cpu->x[1] = cpu->pc + 2;
    cpu->pc = rs1;
    cpu->pc -= 2;
}
void rv32_i_cadd(struct rv32_state* cpu) {
    uint32_t s2 = RV32_CR_RS2; assert(s2 != 0);
    uint32_t d = RV32_CR_RD;

    // printf("%08x: %04x     %-10s x%d, x%d\n", cpu->pc, cpu->opcode & 0xffff, "c.add", d, s2);

    cpu->x[d] += cpu->x[s2];
}
void rv32_i_cfsdsp(struct rv32_state* cpu) { puts("rv32: cfsdsp unimplemented"); exit(1); }
void rv32_i_cswsp(struct rv32_state* cpu) {
    uint32_t imm = RV32_CSS_IMM;
    uint32_t s2 = RV32_CSS_RS2;

    // printf("%08x: %04x     %-10s x%d, %d\n", cpu->pc, cpu->opcode & 0xffff, "c.swsp", s2, imm);

    rv32_bus_write32(cpu, cpu->x[2] + imm, cpu->x[s2]);
}
void rv32_i_cfswsp(struct rv32_state* cpu) { puts("rv32: cfswsp unimplemented"); exit(1); }