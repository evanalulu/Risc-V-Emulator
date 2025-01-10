# RISC-V Emulator with Dynamic Analysis and Cache Simulation

This project implements an emulator in C for a subset of the RISC-V Instruction Set Architecture (ISA). It supports dynamic analysis of instruction execution and includes a processor cache simulator to analyze memory access patterns.

---

## Features

### **1. RISC-V Instruction Emulation**
The emulator decodes and executes a subset of RISC-V instructions, simulating the behavior of assembly programs. Supported RISC-V assembly programs include:
- **Provided Programs**:
  - `quadratic_s`
  - `midpoint_s`
  - `max3_s`
  - `to_upper`
  - `get_bitseq_s`
  - `get_bitseq_signed_s`
  - `countll_s`
  - `swap_s`
  - `sort_s`
- **Your Implementation**:
  - `fib_rec_s`

### **2. Dynamic Analysis**
The emulator collects runtime metrics to analyze instruction execution, including:
- Total instructions executed (`i_count`)
- I-type and R-type instructions executed (`ir_count`)
- Load instructions executed (`ld_count`)
- Store instructions executed (`st_count`)
- Jump instructions executed (`j_count`)
- Conditional branches taken (`b_taken`)
- Conditional branches not taken (`b_not_taken`)

### **3. Cache Simulation**
The project includes a processor cache simulator supporting the following configurations:
- Direct-mapped cache with a block size of 1 word (provided)
- Direct-mapped cache with a block size of 4 words
- 4-way set associative cache with a block size of 1 word and LRU slot replacement
- 4-way set associative cache with a block size of 4 words and LRU slot replacement

---

## Files

### Main Files
- **`emu.c`**: Core file for RISC-V emulation and dynamic analysis.
- **`cache.c`**: Implementation of cache simulation logic.
- **`rv_emu.h`**: Header file for RISC-V state and function definitions.

### Supporting Files
- **`bits.c`** and **`bits.h`**: Utility functions for bit-level manipulations.
- **Program Files**:
  - `fib_rec_s.c`, `quadratic_s.c`, `midpoint_s.c`, etc.: RISC-V programs for testing the emulator.

---

## How to Run

1. **Compile the Emulator**:
   Use the provided `Makefile` to compile the project:
   ```bash
   make
