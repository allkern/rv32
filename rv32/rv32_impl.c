#include <stdlib.h>
#include <stdio.h>

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
                   
#define SE32(v, b) (((int32_t)((v) << (32 - b))) >> (32 - b))

void rv32_i_add(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-8s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "add", d, s1, s2);

    cpu->x[d] = cpu->x[s1] + cpu->x[s2];
}
void rv32_i_sub(struct rv32_state* cpu) {
    uint32_t d = RV32_R_RD;
    uint32_t s1 = RV32_R_RS1;
    uint32_t s2 = RV32_R_RS2;

    // printf("%08x: %08x %-8s x%u, x%u, x%u\n", cpu->pc, cpu->opcode, "sub", d, s1, s2);

    cpu->x[d] = cpu->x[s1] - cpu->x[s2];
}
void rv32_i_xor(struct rv32_state* cpu) { puts("rv32: xor unimplemented"); exit(1); }
void rv32_i_or(struct rv32_state* cpu) { puts("rv32: or unimplemented"); exit(1); }
void rv32_i_and(struct rv32_state* cpu) { puts("rv32: and unimplemented"); exit(1); }
void rv32_i_sll(struct rv32_state* cpu) { puts("rv32: sll unimplemented"); exit(1); }
void rv32_i_srl(struct rv32_state* cpu) { puts("rv32: srl unimplemented"); exit(1); }
void rv32_i_sra(struct rv32_state* cpu) { puts("rv32: sra unimplemented"); exit(1); }
void rv32_i_slt(struct rv32_state* cpu) { puts("rv32: slt unimplemented"); exit(1); }
void rv32_i_sltu(struct rv32_state* cpu) { puts("rv32: sltu unimplemented"); exit(1); }
void rv32_i_addi(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-8s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "addi", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] + SE32(imm, 12);
}
void rv32_i_xori(struct rv32_state* cpu) { puts("rv32: xori unimplemented"); exit(1); }
void rv32_i_ori(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-8s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "ori", d, s1, SE32(imm, 12));

    cpu->x[d] = cpu->x[s1] | SE32(imm, 12);
}
void rv32_i_andi(struct rv32_state* cpu) { puts("rv32: andi unimplemented"); exit(1); }
void rv32_i_slli(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t shamt = RV32_I_IMM & 0x3f;

    // printf("%08x: %08x %-8s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "slli", d, s1, shamt);

    cpu->x[d] = cpu->x[s1] << shamt;
}
void rv32_i_srli(struct rv32_state* cpu) { puts("rv32: srli unimplemented"); exit(1); }
void rv32_i_srai(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t shamt = RV32_I_IMM & 0x3f;

    // printf("%08x: %08x %-8s x%u, x%u, %d\n", cpu->pc, cpu->opcode, "srai", d, s1, shamt);

    cpu->x[d] = ((int32_t)cpu->x[s1]) >> shamt;
}
void rv32_i_slti(struct rv32_state* cpu) { puts("rv32: slti unimplemented"); exit(1); }
void rv32_i_sltiu(struct rv32_state* cpu) { puts("rv32: sltiu unimplemented"); exit(1); }
void rv32_i_lb(struct rv32_state* cpu) { puts("rv32: lb unimplemented"); exit(1); }
void rv32_i_lh(struct rv32_state* cpu) { puts("rv32: lh unimplemented"); exit(1); }
void rv32_i_lw(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-8s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lw", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read32(cpu, cpu->x[s1] + SE32(imm, 12));
}
void rv32_i_lbu(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-8s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "lbu", d, SE32(imm, 12), s1);

    cpu->x[d] = rv32_bus_read8(cpu, cpu->x[s1] + SE32(imm, 12)) & 0xff;
}
void rv32_i_lhu(struct rv32_state* cpu) { puts("rv32: lhu unimplemented"); exit(1); }
void rv32_i_sb(struct rv32_state* cpu) {
    uint32_t s1 = RV32_S_RS1;
    uint32_t s2 = RV32_S_RS2;
    uint32_t imm = RV32_S_IMM;

    rv32_bus_write8(cpu, cpu->x[s1] + SE32(imm, 12), cpu->x[s2] & 0xff);

    // printf("%08x: %08x %-8s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "sb", s2, SE32(imm, 12), s1);
}
void rv32_i_sh(struct rv32_state* cpu) { puts("rv32: sh unimplemented"); exit(1); }
void rv32_i_sw(struct rv32_state* cpu) {
    uint32_t s1 = RV32_S_RS1;
    uint32_t s2 = RV32_S_RS2;
    uint32_t imm = RV32_S_IMM;

    rv32_bus_write32(cpu, cpu->x[s1] + SE32(imm, 12), cpu->x[s2]);

    // printf("%08x: %08x %-8s x%u, %d(x%u)\n", cpu->pc, cpu->opcode, "sw", s2, SE32(imm, 12), s1);
}
void rv32_i_beq(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-8s x%u, x%u, %x\n", cpu->pc, cpu->opcode, "bne", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] == cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bne(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-8s x%u, x%u, %x\n", cpu->pc, cpu->opcode, "bne", s1, s2, cpu->pc + SE32(imm, 13));

    if (cpu->x[s1] != cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_blt(struct rv32_state* cpu) {
    uint32_t s1 = RV32_B_RS1;
    uint32_t s2 = RV32_B_RS2;
    uint32_t imm = RV32_B_IMM;

    // printf("%08x: %08x %-8s x%u, x%u, %x\n", cpu->pc, cpu->opcode, "blt", s1, s2, cpu->pc + SE32(imm, 13));

    if ((int32_t)cpu->x[s1] < (int32_t)cpu->x[s2])
        cpu->pc = (cpu->pc - 4) + SE32(imm, 13);
}
void rv32_i_bge(struct rv32_state* cpu) { puts("rv32: bge unimplemented"); exit(1); }
void rv32_i_bltu(struct rv32_state* cpu) { puts("rv32: bltu unimplemented"); exit(1); }
void rv32_i_bgeu(struct rv32_state* cpu) { puts("rv32: bgeu unimplemented"); exit(1); }
void rv32_i_jal(struct rv32_state* cpu) {
    uint32_t d = RV32_J_RD;
    uint32_t imm = RV32_J_IMM;

    // printf("%08x: %08x %-8s x%u, %x\n", cpu->pc, cpu->opcode, "jal", d, cpu->pc + SE32(imm, 21));

    cpu->x[d] = cpu->pc + 4;
    cpu->pc = (cpu->pc - 4) + SE32(imm, 21);
}
void rv32_i_jalr(struct rv32_state* cpu) {
    uint32_t d = RV32_I_RD;
    uint32_t s1 = RV32_I_RS1;
    uint32_t imm = RV32_I_IMM;

    // printf("%08x: %08x %-8s x%u, %x\n", cpu->pc, cpu->opcode, "jalr", d, (cpu->x[s1] + SE32(imm, 12)) & 0xfffffffe);

    cpu->x[d] = cpu->pc + 4;
    cpu->pc = ((cpu->x[s1] + SE32(imm, 12)) & 0xfffffffe) - 4;
}
void rv32_i_lui(struct rv32_state* cpu) {
    uint32_t d = RV32_U_RD;
    uint32_t imm = RV32_U_IMM;

    // printf("%08x: %08x %-8s x%u, %x\n", cpu->pc, cpu->opcode, "lui", d, imm);

    cpu->x[d] = imm << 12;
}
void rv32_i_auipc(struct rv32_state* cpu) {
    uint32_t d = RV32_U_RD;
    uint32_t imm = RV32_U_IMM;

    // printf("%08x: %08x %-8s x%u, %x\n", cpu->pc, cpu->opcode, "auipc", d, cpu->pc + (imm << 12));

    cpu->x[d] = cpu->pc + (imm << 12);
}
void rv32_i_ecall(struct rv32_state* cpu) {
    if (cpu->x[17] != 93)
        return;

    if (cpu->x[10]) {
        // printf("fail!\n");
    } else {
        // printf("pass!\n");
    }

    exit(1);
}
void rv32_i_ebreak(struct rv32_state* cpu) { puts("rv32: ebreak unimplemented"); exit(1); }
void rv32_i_mul(struct rv32_state* cpu) { puts("rv32: mul unimplemented"); exit(1); }
void rv32_i_mulh(struct rv32_state* cpu) { puts("rv32: mulh unimplemented"); exit(1); }
void rv32_i_mulhsu(struct rv32_state* cpu) { puts("rv32: mulhsu unimplemented"); exit(1); }
void rv32_i_mulhu(struct rv32_state* cpu) { puts("rv32: mulhu unimplemented"); exit(1); }
void rv32_i_div(struct rv32_state* cpu) { puts("rv32: div unimplemented"); exit(1); }
void rv32_i_divu(struct rv32_state* cpu) { puts("rv32: divu unimplemented"); exit(1); }
void rv32_i_rem(struct rv32_state* cpu) { puts("rv32: rem unimplemented"); exit(1); }
void rv32_i_remu(struct rv32_state* cpu) { puts("rv32: remu unimplemented"); exit(1); }
void rv32_i_amoadd(struct rv32_state* cpu) { puts("rv32: amoadd unimplemented"); exit(1); }
void rv32_i_amoswap(struct rv32_state* cpu) { puts("rv32: amoswap unimplemented"); exit(1); }
void rv32_i_lr(struct rv32_state* cpu) { puts("rv32: lr unimplemented"); exit(1); }
void rv32_i_sc(struct rv32_state* cpu) { puts("rv32: sc unimplemented"); exit(1); }
void rv32_i_amoxor(struct rv32_state* cpu) { puts("rv32: amoxor unimplemented"); exit(1); }
void rv32_i_amoor(struct rv32_state* cpu) { puts("rv32: amoor unimplemented"); exit(1); }
void rv32_i_amoand(struct rv32_state* cpu) { puts("rv32: amoand unimplemented"); exit(1); }
void rv32_i_amomin(struct rv32_state* cpu) { puts("rv32: amomin unimplemented"); exit(1); }
void rv32_i_amomax(struct rv32_state* cpu) { puts("rv32: amomax unimplemented"); exit(1); }
void rv32_i_amominu(struct rv32_state* cpu) { puts("rv32: amominu unimplemented"); exit(1); }
void rv32_i_amomaxu(struct rv32_state* cpu) { puts("rv32: amomaxu unimplemented"); exit(1); }
void rv32_i_fence(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;

    // NOP for now
    // printf("%08x: %08x %-8s x%u, x%u\n", cpu->pc, cpu->opcode, "fence", d, s1);
}
void rv32_i_fencei(struct rv32_state* cpu) { puts("rv32: fencei unimplemented"); exit(1); }
void rv32_i_csrrw(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    // NOP for now
    // printf("%08x: %08x %-8s x%u, x%u, %x\n", cpu->pc, cpu->opcode, "csrrw", d, s1, csr);
}
void rv32_i_csrrs(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    // NOP for now
    // printf("%08x: %08x %-8s x%u, x%u, %x\n", cpu->pc, cpu->opcode, "csrrs", d, s1, csr);
}
void rv32_i_csrrc(struct rv32_state* cpu) { puts("rv32: csrrc unimplemented"); exit(1); }
void rv32_i_csrrwi(struct rv32_state* cpu) {
    uint32_t d = RV32_CSR_RD;
    uint32_t s1 = RV32_CSR_RS1;
    uint32_t csr = RV32_CSR_CSR;

    // NOP for now
    // printf("%08x: %08x %-8s x%u, %x, %x\n", cpu->pc, cpu->opcode, "csrrwi", d, s1, csr);
}
void rv32_i_csrrsi(struct rv32_state* cpu) { puts("rv32: csrrsi unimplemented"); exit(1); }
void rv32_i_csrrci(struct rv32_state* cpu) { puts("rv32: csrrci unimplemented"); exit(1); }
void rv32_i_flw(struct rv32_state* cpu) { puts("rv32: flw unimplemented"); exit(1); }
void rv32_i_fsw(struct rv32_state* cpu) { puts("rv32: fsw unimplemented"); exit(1); }
void rv32_i_fmadds(struct rv32_state* cpu) { puts("rv32: fmadds unimplemented"); exit(1); }
void rv32_i_fmsubs(struct rv32_state* cpu) { puts("rv32: fmsubs unimplemented"); exit(1); }
void rv32_i_fnmsubs(struct rv32_state* cpu) { puts("rv32: fnmsubs unimplemented"); exit(1); }
void rv32_i_fnmadds(struct rv32_state* cpu) { puts("rv32: fnmadds unimplemented"); exit(1); }
void rv32_i_fadds(struct rv32_state* cpu) { puts("rv32: fadds unimplemented"); exit(1); }
void rv32_i_fsubs(struct rv32_state* cpu) { puts("rv32: fsubs unimplemented"); exit(1); }
void rv32_i_fmuls(struct rv32_state* cpu) { puts("rv32: fmuls unimplemented"); exit(1); }
void rv32_i_fdivs(struct rv32_state* cpu) { puts("rv32: fdivs unimplemented"); exit(1); }
void rv32_i_fsqrts(struct rv32_state* cpu) { puts("rv32: fsqrts unimplemented"); exit(1); }
void rv32_i_fsgnjs(struct rv32_state* cpu) { puts("rv32: fsgnjs unimplemented"); exit(1); }
void rv32_i_fsgnjns(struct rv32_state* cpu) { puts("rv32: fsgnjns unimplemented"); exit(1); }
void rv32_i_fsgnjxs(struct rv32_state* cpu) { puts("rv32: fsgnjxs unimplemented"); exit(1); }
void rv32_i_fmins(struct rv32_state* cpu) { puts("rv32: fmins unimplemented"); exit(1); }
void rv32_i_fmaxs(struct rv32_state* cpu) { puts("rv32: fmaxs unimplemented"); exit(1); }
void rv32_i_fcvtws(struct rv32_state* cpu) { puts("rv32: fcvtws unimplemented"); exit(1); }
void rv32_i_fcvtwus(struct rv32_state* cpu) { puts("rv32: fcvtwus unimplemented"); exit(1); }
void rv32_i_fmvxw(struct rv32_state* cpu) { puts("rv32: fmvxw unimplemented"); exit(1); }
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
    // printf("%08x: %08x %-8s\n", cpu->pc, cpu->opcode, "mret");
}
void rv32_i_mnret(struct rv32_state* cpu) { puts("rv32: mnret unimplemented"); exit(1); }
void rv32_i_wfi(struct rv32_state* cpu) { puts("rv32: wfi unimplemented"); exit(1); }