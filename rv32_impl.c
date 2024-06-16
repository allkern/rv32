#include <stdlib.h>
#include <stdio.h>

#include "rv32.h"

#define RV32_U_IMM ((cpu->opcode >> 12) & 0xfffff)
#define RV32_U_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_I_RD ((cpu->opcode >> 7) & 0x1f)
#define RV32_I_RS1 ((cpu->opcode >> 15) & 0x1f)
#define RV32_I_IMM ((cpu->opcode >> 20) & 0xfff)
#define SE32(v, b) (((int32_t)((v) << (32 - b))) >> (32 - b))

void rv32_i_add(struct rv32_state* cpu) { puts("rv32: add unimplemented"); exit(1); }
void rv32_i_sub(struct rv32_state* cpu) { puts("rv32: sub unimplemented"); exit(1); }
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

    cpu->x[d] = cpu->x[s1] + SE32(imm, 12);
}
void rv32_i_xori(struct rv32_state* cpu) { puts("rv32: xori unimplemented"); exit(1); }
void rv32_i_ori(struct rv32_state* cpu) { puts("rv32: ori unimplemented"); exit(1); }
void rv32_i_andi(struct rv32_state* cpu) { puts("rv32: andi unimplemented"); exit(1); }
void rv32_i_slli(struct rv32_state* cpu) { puts("rv32: slli unimplemented"); exit(1); }
void rv32_i_srli(struct rv32_state* cpu) { puts("rv32: srli unimplemented"); exit(1); }
void rv32_i_srai(struct rv32_state* cpu) { puts("rv32: srai unimplemented"); exit(1); }
void rv32_i_slti(struct rv32_state* cpu) { puts("rv32: slti unimplemented"); exit(1); }
void rv32_i_sltiu(struct rv32_state* cpu) { puts("rv32: sltiu unimplemented"); exit(1); }
void rv32_i_lb(struct rv32_state* cpu) { puts("rv32: lb unimplemented"); exit(1); }
void rv32_i_lh(struct rv32_state* cpu) { puts("rv32: lh unimplemented"); exit(1); }
void rv32_i_lw(struct rv32_state* cpu) { puts("rv32: lw unimplemented"); exit(1); }
void rv32_i_lbu(struct rv32_state* cpu) { puts("rv32: lbu unimplemented"); exit(1); }
void rv32_i_lhu(struct rv32_state* cpu) { puts("rv32: lhu unimplemented"); exit(1); }
void rv32_i_sb(struct rv32_state* cpu) { puts("rv32: sb unimplemented"); exit(1); }
void rv32_i_sh(struct rv32_state* cpu) { puts("rv32: sh unimplemented"); exit(1); }
void rv32_i_sw(struct rv32_state* cpu) { puts("rv32: sw unimplemented"); exit(1); }
void rv32_i_beq(struct rv32_state* cpu) { puts("rv32: beq unimplemented"); exit(1); }
void rv32_i_bne(struct rv32_state* cpu) { puts("rv32: bne unimplemented"); exit(1); }
void rv32_i_blt(struct rv32_state* cpu) { puts("rv32: blt unimplemented"); exit(1); }
void rv32_i_bge(struct rv32_state* cpu) { puts("rv32: bge unimplemented"); exit(1); }
void rv32_i_bltu(struct rv32_state* cpu) { puts("rv32: bltu unimplemented"); exit(1); }
void rv32_i_bgeu(struct rv32_state* cpu) { puts("rv32: bgeu unimplemented"); exit(1); }
void rv32_i_jal(struct rv32_state* cpu) { puts("rv32: jal unimplemented"); exit(1); }
void rv32_i_jalr(struct rv32_state* cpu) { puts("rv32: jalr unimplemented"); exit(1); }
void rv32_i_lui(struct rv32_state* cpu) { puts("rv32: lui unimplemented"); exit(1); }
void rv32_i_auipc(struct rv32_state* cpu) {
    uint32_t imm = RV32_U_IMM;
    uint32_t d = RV32_U_RD;

    cpu->x[d] = cpu->pc + imm;

    printf("auipc x%u, %08x\n", d, imm);
}
void rv32_i_ecall(struct rv32_state* cpu) { puts("rv32: ecall unimplemented"); exit(1); }
void rv32_i_ebreak(struct rv32_state* cpu) { puts("rv32: ebreak unimplemented"); exit(1); }
void rv32_i_mul(struct rv32_state* cpu) { puts("rv32: mul unimplemented"); exit(1); }
void rv32_i_mulh(struct rv32_state* cpu) { puts("rv32: mulh unimplemented"); exit(1); }
void rv32_i_mulsu(struct rv32_state* cpu) { puts("rv32: mulsu unimplemented"); exit(1); }
void rv32_i_mulu(struct rv32_state* cpu) { puts("rv32: mulu unimplemented"); exit(1); }
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
void rv32_i_fencei(struct rv32_state* cpu) { puts("rv32: fencei unimplemented"); exit(1); }
void rv32_i_csrrw(struct rv32_state* cpu) { puts("rv32: csrrw unimplemented"); exit(1); }
void rv32_i_csrrs(struct rv32_state* cpu) { puts("rv32: csrrs unimplemented"); exit(1); }
void rv32_i_csrrc(struct rv32_state* cpu) { puts("rv32: csrrc unimplemented"); exit(1); }
void rv32_i_csrrwi(struct rv32_state* cpu) { puts("rv32: csrrwi unimplemented"); exit(1); }
void rv32_i_csrrsi(struct rv32_state* cpu) { puts("rv32: csrrsi unimplemented"); exit(1); }
void rv32_i_csrrci(struct rv32_state* cpu) { puts("rv32: csrrci unimplemented"); exit(1); }