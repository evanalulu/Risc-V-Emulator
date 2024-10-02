#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rv_emu.h"
#include "bits.h"

#define DEBUG 0

static void unsupported(char *s, uint32_t n) {
    printf("unsupported %s 0x%x\n", s, n);
    exit(-1);
}

static uint32_t get_opcode(uint32_t iw) {
    return get_bits(iw, 0, 7);
}

static uint32_t get_rd(uint32_t iw) {
    return get_bits(iw, 7, 5);
}

static uint32_t get_funct3(uint32_t iw) {
    return get_bits(iw, 12, 3);
}

static uint32_t get_funct7(uint32_t iw) {
    return get_bits(iw, 25, 7);
}

static uint32_t get_rs1(uint32_t iw) {
    return get_bits(iw, 15, 5);
}

static uint32_t get_rs2(uint32_t iw) {
    return get_bits(iw, 20, 5);
}

void emu_r_type(struct rv_state *rsp, uint32_t iw) {
    uint32_t rd = get_rd(iw);
    uint32_t rs1 = get_rs1(iw);
    uint32_t rs2 = get_rs2(iw);
    uint32_t funct3 = get_funct3(iw);
    uint32_t funct7 = get_funct7(iw);

    if (funct3 == 0b000 && funct7 == 0b0000000) {
        // ADD
        rsp->regs[rd] = rsp->regs[rs1] + rsp->regs[rs2];
    } else if (funct3 == 0b000 && funct7 == 0b0100000) {
        // SUB
        rsp->regs[rd] = rsp->regs[rs1] - rsp->regs[rs2];
    } else if (funct3 == 0b000 && funct7 == 0b0000001) {
        // MULT
        rsp->regs[rd] = rsp->regs[rs1] * rsp->regs[rs2];
    } else if (funct3 == 0b111 && funct7 == 0b0000000) {
        // AND
        rsp-> regs[rd] = rsp->regs[rs1] & rsp->regs[rs2];
    } else if (funct3 == 0b001 && funct7 == 0b0000000) {
        // SLL
        rsp->regs[rd] = rsp->regs[rs1] << rsp->regs[rs2];
    } else if (funct3 == 0b101 && funct7 == 0b0000000) {
        // SRL
        rsp->regs[rd] = rsp->regs[rs1] >> rsp->regs[rs2];
    } else {
        unsupported("R-type funct3", funct3);
    }

    rsp->pc += 4; // Next instruction
}

void emu_i_arith(struct rv_state *rsp, uint32_t iw) {
    uint32_t rd = get_rd(iw);
    uint32_t rs1 = get_rs1(iw);
    uint32_t imm_unsigned = get_bits(iw, 20, 12);
    int64_t imm = sign_extend(imm_unsigned, 12);
     
    uint32_t funct3 = get_funct3(iw);
    uint32_t funct7 = get_funct7(iw);
    
    if (funct3 == 0b000) {
        // ADDI
        rsp->regs[rd] = rsp->regs[rs1] + imm;
    } else if (funct3 == 0b101 && funct7 == 0b0000000) {
        // SRLI
        rsp->regs[rd] = rsp->regs[rs1] >> imm;
    } else {
        unsupported("I-type funct3", funct3);
    }
    
     rsp->pc += 4; // Next instruction
}

void emu_b_type(struct rv_state *rsp, uint32_t iw) {
    uint32_t rs1 = get_rs1(iw);
    uint32_t rs2 = get_rs2(iw);
    uint32_t funct3 = get_funct3(iw);

    uint64_t imm11_5, imm4_0, uimm;
    int64_t imm;

    imm11_5 = get_bits(iw, 25, 7);
    imm4_0 = get_bits(iw, 7, 5);
    uimm = (imm11_5 << 5) | imm4_0;
    imm = sign_extend(uimm, 12);
        
    if (funct3 == 0b100) {
        // BLT & BGT
        if ((int64_t)rsp->regs[rs1] < (int64_t)rsp->regs[rs2]) {
            rsp->pc += imm;            
        } else {
            rsp->pc += 4;
        }
    } else if (funct3 == 0b001) {
        // BNE
        if ((int64_t)rsp->regs[rs1] != (int64_t)rsp-> regs[rs2]) {
            rsp->pc += imm;
        } else {
            rsp-> pc += 4;
        }
    } else {
        unsupported("B-type funct3", funct3);
    }

}

void emu_jalr(struct rv_state *rsp, uint32_t iw) {
    uint32_t rd = get_rd(iw);
    uint32_t rs1 = get_rs1(iw);
    uint32_t imm_unsigned = get_bits(iw, 20, 12);
    int64_t imm = sign_extend(imm_unsigned, 12);
    
    if (rd != 0) {
        rsp->regs[RV_RA] = rsp->pc + 4;
    }

    rsp->pc = rsp->regs[rs1] + imm;
}

static void rv_one(struct rv_state *state) {
    uint32_t iw  = *((uint32_t*) state->pc);
    //iw = cache_lookup(&state->i_cache, (uint64_t) state->pc);

    uint32_t opcode = get_opcode(iw);

    switch (opcode) {
        case FMT_R:
            // R-type
            emu_r_type(state, iw);
            break;
        case FMT_I_ARITH:
            // I-type
            emu_i_arith(state, iw);
            break;
        case FMT_SB:
            // B-type
            emu_b_type(state, iw);
            break;
        case FMT_I_JALR:
            // JALR (aka RET) is a variant of I-type instructions
            emu_jalr(state, iw);
            break;
        default:
            unsupported("Unknown opcode: ", opcode);
                
    }

#if DEBUG
    printf("iw: %08x\n", iw);
#endif

}

void rv_init(struct rv_state *state, uint32_t *target, 
             uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {

    // Initialize the Program Counter
    // rsp->pc = (uint64_t) func;
    
    state->pc = (uint64_t) target;
    state->regs[RV_A0] = a0;
    state->regs[RV_A1] = a1;
    state->regs[RV_A2] = a2;
    state->regs[RV_A3] = a3;

    state->regs[RV_ZERO] = 0;  // zero is always 0  (:
    state->regs[RV_RA] = RV_STOP;
    state->regs[RV_SP] = (uint64_t) &state->stack[STACK_SIZE];

    memset(&state->analysis, 0, sizeof(struct rv_analysis_st));
    cache_init(&state->i_cache);
}

uint64_t rv_emulate(struct rv_state *state) {
    while (state->pc != RV_STOP) {
        rv_one(state);
    }
    return state->regs[RV_A0];
}

static void print_pct(char *fmt, int numer, int denom) {
    double pct = 0.0;

    if (denom)
        pct = (double) numer / (double) denom * 100.0;
    printf(fmt, numer, pct);
}

void rv_print(struct rv_analysis_st *a) {
    int b_total = a->b_taken + a->b_not_taken;

    printf("=== Analysis\n");
    print_pct("Instructions Executed  = %d\n", a->i_count, a->i_count);
    print_pct("R-type + I-type        = %d (%.2f%%)\n", a->ir_count, a->i_count);
    print_pct("Loads                  = %d (%.2f%%)\n", a->ld_count, a->i_count);
    print_pct("Stores                 = %d (%.2f%%)\n", a->st_count, a->i_count);    
    print_pct("Jumps/JAL/JALR         = %d (%.2f%%)\n", a->j_count, a->i_count);
    print_pct("Conditional branches   = %d (%.2f%%)\n", b_total, a->i_count);
    print_pct("  Branches taken       = %d (%.2f%%)\n", a->b_taken, b_total);
    print_pct("  Branches not taken   = %d (%.2f%%)\n", a->b_not_taken, b_total);
}
