#ifndef RV32_H
#define RV32_H

#include <stdint.h>

typedef uint32_t (*rv32_bus_read32_func)(void*, uint32_t);
typedef uint32_t (*rv32_bus_read16_func)(void*, uint32_t);
typedef uint32_t (*rv32_bus_read8_func)(void*, uint32_t);
typedef void (*rv32_bus_write32_func)(void*, uint32_t, uint32_t);
typedef void (*rv32_bus_write16_func)(void*, uint32_t, uint32_t);
typedef void (*rv32_bus_write8_func)(void*, uint32_t, uint32_t);

struct rv32_bus {
    rv32_bus_read32_func read32;
    rv32_bus_read16_func read16;
    rv32_bus_read8_func read8;
    rv32_bus_write32_func write32;
    rv32_bus_write16_func write16;
    rv32_bus_write8_func write8;
    void* udata;
};

struct rv32_state {
    struct rv32_bus bus;

    uint32_t x[32];
    uint32_t pc;
    uint32_t opcode;

    // LR/SC reservation
    uint32_t res_value;
    uint32_t res_addr;
    int res_valid;

    union {
        uint64_t u64;
        double d;
        uint32_t u32;
        float f;
    } f[32];

    uint32_t fcsr;
};

struct rv32_state* rv32_create(void);
void rv32_init(struct rv32_state* cpu, struct rv32_bus bus);
int rv32_execute(struct rv32_state* cpu, uint32_t opcode);
void rv32_cycle(struct rv32_state* cpu);
void rv32_destroy(struct rv32_state* cpu);

void rv32_set_pc(struct rv32_state* cpu, uint32_t pc);
uint32_t rv32_bus_read32(struct rv32_state* cpu, uint32_t addr);
uint32_t rv32_bus_read16(struct rv32_state* cpu, uint32_t addr);
uint32_t rv32_bus_read8(struct rv32_state* cpu, uint32_t addr);
void rv32_bus_write32(struct rv32_state* cpu, uint32_t addr, uint32_t data);
void rv32_bus_write16(struct rv32_state* cpu, uint32_t addr, uint32_t data);
void rv32_bus_write8(struct rv32_state* cpu, uint32_t addr, uint32_t data);

void rv32_i_add(struct rv32_state* cpu);
void rv32_i_sub(struct rv32_state* cpu);
void rv32_i_xor(struct rv32_state* cpu);
void rv32_i_or(struct rv32_state* cpu);
void rv32_i_and(struct rv32_state* cpu);
void rv32_i_sll(struct rv32_state* cpu);
void rv32_i_srl(struct rv32_state* cpu);
void rv32_i_sra(struct rv32_state* cpu);
void rv32_i_slt(struct rv32_state* cpu);
void rv32_i_sltu(struct rv32_state* cpu);
void rv32_i_addi(struct rv32_state* cpu);
void rv32_i_xori(struct rv32_state* cpu);
void rv32_i_ori(struct rv32_state* cpu);
void rv32_i_andi(struct rv32_state* cpu);
void rv32_i_slli(struct rv32_state* cpu);
void rv32_i_srli(struct rv32_state* cpu);
void rv32_i_srai(struct rv32_state* cpu);
void rv32_i_slti(struct rv32_state* cpu);
void rv32_i_sltiu(struct rv32_state* cpu);
void rv32_i_lb(struct rv32_state* cpu);
void rv32_i_lh(struct rv32_state* cpu);
void rv32_i_lw(struct rv32_state* cpu);
void rv32_i_lbu(struct rv32_state* cpu);
void rv32_i_lhu(struct rv32_state* cpu);
void rv32_i_sb(struct rv32_state* cpu);
void rv32_i_sh(struct rv32_state* cpu);
void rv32_i_sw(struct rv32_state* cpu);
void rv32_i_beq(struct rv32_state* cpu);
void rv32_i_bne(struct rv32_state* cpu);
void rv32_i_blt(struct rv32_state* cpu);
void rv32_i_bge(struct rv32_state* cpu);
void rv32_i_bltu(struct rv32_state* cpu);
void rv32_i_bgeu(struct rv32_state* cpu);
void rv32_i_jal(struct rv32_state* cpu);
void rv32_i_jalr(struct rv32_state* cpu);
void rv32_i_lui(struct rv32_state* cpu);
void rv32_i_auipc(struct rv32_state* cpu);
void rv32_i_ecall(struct rv32_state* cpu);
void rv32_i_ebreak(struct rv32_state* cpu);
void rv32_i_mul(struct rv32_state* cpu);
void rv32_i_mulh(struct rv32_state* cpu);
void rv32_i_mulhsu(struct rv32_state* cpu);
void rv32_i_mulhu(struct rv32_state* cpu);
void rv32_i_div(struct rv32_state* cpu);
void rv32_i_divu(struct rv32_state* cpu);
void rv32_i_rem(struct rv32_state* cpu);
void rv32_i_remu(struct rv32_state* cpu);
void rv32_i_amoadd(struct rv32_state* cpu);
void rv32_i_amoswap(struct rv32_state* cpu);
void rv32_i_lr(struct rv32_state* cpu);
void rv32_i_sc(struct rv32_state* cpu);
void rv32_i_amoxor(struct rv32_state* cpu);
void rv32_i_amoor(struct rv32_state* cpu);
void rv32_i_amoand(struct rv32_state* cpu);
void rv32_i_amomin(struct rv32_state* cpu);
void rv32_i_amomax(struct rv32_state* cpu);
void rv32_i_amominu(struct rv32_state* cpu);
void rv32_i_amomaxu(struct rv32_state* cpu);
void rv32_i_fence(struct rv32_state* cpu);
void rv32_i_fencei(struct rv32_state* cpu);
void rv32_i_csrrw(struct rv32_state* cpu);
void rv32_i_csrrs(struct rv32_state* cpu);
void rv32_i_csrrc(struct rv32_state* cpu);
void rv32_i_csrrwi(struct rv32_state* cpu);
void rv32_i_csrrsi(struct rv32_state* cpu);
void rv32_i_csrrci(struct rv32_state* cpu);
void rv32_i_flw(struct rv32_state* cpu);
void rv32_i_fsw(struct rv32_state* cpu);
void rv32_i_fmadds(struct rv32_state* cpu);
void rv32_i_fmsubs(struct rv32_state* cpu);
void rv32_i_fnmsubs(struct rv32_state* cpu);
void rv32_i_fnmadds(struct rv32_state* cpu);
void rv32_i_fadds(struct rv32_state* cpu);
void rv32_i_fsubs(struct rv32_state* cpu);
void rv32_i_fmuls(struct rv32_state* cpu);
void rv32_i_fdivs(struct rv32_state* cpu);
void rv32_i_fsqrts(struct rv32_state* cpu);
void rv32_i_fsgnjs(struct rv32_state* cpu);
void rv32_i_fsgnjns(struct rv32_state* cpu);
void rv32_i_fsgnjxs(struct rv32_state* cpu);
void rv32_i_fmins(struct rv32_state* cpu);
void rv32_i_fmaxs(struct rv32_state* cpu);
void rv32_i_fcvtws(struct rv32_state* cpu);
void rv32_i_fcvtwus(struct rv32_state* cpu);
void rv32_i_fmvxw(struct rv32_state* cpu);
void rv32_i_fles(struct rv32_state* cpu);
void rv32_i_flts(struct rv32_state* cpu);
void rv32_i_feqs(struct rv32_state* cpu);
void rv32_i_fclasss(struct rv32_state* cpu);
void rv32_i_fcvtsw(struct rv32_state* cpu);
void rv32_i_fcvtswu(struct rv32_state* cpu);
void rv32_i_fmvwx(struct rv32_state* cpu);
void rv32_i_fld(struct rv32_state* cpu);
void rv32_i_fsd(struct rv32_state* cpu);
void rv32_i_fmaddd(struct rv32_state* cpu);
void rv32_i_fmsubd(struct rv32_state* cpu);
void rv32_i_fnmsubd(struct rv32_state* cpu);
void rv32_i_fnmaddd(struct rv32_state* cpu);
void rv32_i_faddd(struct rv32_state* cpu);
void rv32_i_fsubd(struct rv32_state* cpu);
void rv32_i_fmuld(struct rv32_state* cpu);
void rv32_i_fdivd(struct rv32_state* cpu);
void rv32_i_fsqrtd(struct rv32_state* cpu);
void rv32_i_fsgnjd(struct rv32_state* cpu);
void rv32_i_fsgnjnd(struct rv32_state* cpu);
void rv32_i_fsgnjxd(struct rv32_state* cpu);
void rv32_i_fmind(struct rv32_state* cpu);
void rv32_i_fmaxd(struct rv32_state* cpu);
void rv32_i_fcvtsd(struct rv32_state* cpu);
void rv32_i_fcvtds(struct rv32_state* cpu);
void rv32_i_fled(struct rv32_state* cpu);
void rv32_i_fltd(struct rv32_state* cpu);
void rv32_i_feqd(struct rv32_state* cpu);
void rv32_i_fclassd(struct rv32_state* cpu);
void rv32_i_fcvtwd(struct rv32_state* cpu);
void rv32_i_fcvtwud(struct rv32_state* cpu);
void rv32_i_fcvtdw(struct rv32_state* cpu);
void rv32_i_fcvtdwu(struct rv32_state* cpu);
void rv32_i_sret(struct rv32_state* cpu);
void rv32_i_mret(struct rv32_state* cpu);
void rv32_i_mnret(struct rv32_state* cpu);
void rv32_i_wfi(struct rv32_state* cpu);

void rv32_i_caddi4spn(struct rv32_state* cpu);
void rv32_i_cfld(struct rv32_state* cpu);
void rv32_i_clw(struct rv32_state* cpu);
void rv32_i_cflw(struct rv32_state* cpu);
void rv32_i_cfsd(struct rv32_state* cpu);
void rv32_i_csw(struct rv32_state* cpu);
void rv32_i_cfsw(struct rv32_state* cpu);
void rv32_i_cnop(struct rv32_state* cpu);
void rv32_i_caddi(struct rv32_state* cpu);
void rv32_i_cjal(struct rv32_state* cpu);
void rv32_i_cli(struct rv32_state* cpu);
void rv32_i_caddi16sp(struct rv32_state* cpu);
void rv32_i_clui(struct rv32_state* cpu);
void rv32_i_csrli(struct rv32_state* cpu);
void rv32_i_csrai(struct rv32_state* cpu);
void rv32_i_candi(struct rv32_state* cpu);
void rv32_i_csub(struct rv32_state* cpu);
void rv32_i_cxor(struct rv32_state* cpu);
void rv32_i_cor(struct rv32_state* cpu);
void rv32_i_cand(struct rv32_state* cpu);
void rv32_i_cj(struct rv32_state* cpu);
void rv32_i_cbeqz(struct rv32_state* cpu);
void rv32_i_cbnez(struct rv32_state* cpu);
void rv32_i_cslli(struct rv32_state* cpu);
void rv32_i_cfldsp(struct rv32_state* cpu);
void rv32_i_clwsp(struct rv32_state* cpu);
void rv32_i_cflwsp(struct rv32_state* cpu);
void rv32_i_cjr(struct rv32_state* cpu);
void rv32_i_cmv(struct rv32_state* cpu);
void rv32_i_cebreak(struct rv32_state* cpu);
void rv32_i_cjalr(struct rv32_state* cpu);
void rv32_i_cadd(struct rv32_state* cpu);
void rv32_i_cfsdsp(struct rv32_state* cpu);
void rv32_i_cswsp(struct rv32_state* cpu);
void rv32_i_cfswsp(struct rv32_state* cpu);

#endif