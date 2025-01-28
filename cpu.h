#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>

#define uchar unsigned char
#define ushort unsigned short
#define MEMORY_SIZE 0xFFFF

typedef struct CPU{
    uchar reg_a;
    uchar reg_x;
    uchar reg_y;
    uchar status;
    ushort pc;
    uchar memory[MEMORY_SIZE];
} CPU;

typedef enum ADDRESS_MODE {
    ADDRESS_IMMEDIATE,
    ADDRESS_ZEROPAGE,
    ADDRESS_ZEROPAGE_X,
    ADDRESS_ZEROPAGE_Y,
    ADDRESS_ABSOLUTE,
    ADDRESS_ABSOLUTE_X,
    ADDRESS_ABSOLUTE_Y,
    ADDRESS_INDIRECT_X,
    ADDRESS_INDIRECT_Y,
    ADDRESS_NONE,
} ADDRESS_MODE;

CPU make_cpu(void);

uchar cpu_read_memory(CPU* cpu, ushort addr);

void cpu_write_memory(CPU* cpu, ushort addr, uchar data);

ushort cpu_read_memory_ushort(CPU* cpu, ushort pos);

void cpu_write_memory_ushort(CPU* cpu, ushort pos, ushort data);

void cpu_reset(CPU* cpu);

void cpu_load(CPU* cpu, uchar *program, size_t program_length);

ushort cpu_get_operand_address(CPU* cpu, ADDRESS_MODE mode);

void cpu_update_zero_and_negative_flags(CPU* cpu, uchar result);

void cpu_instruction_LDA(CPU* cpu, ADDRESS_MODE mode);

void cpu_instruction_TAX(CPU* cpu);

void cpu_instruction_INX(CPU* cpu);

void cpu_instruction_STA(CPU* cpu, ADDRESS_MODE mode);

void cpu_run(CPU* cpu);

void cpu_load_and_run(CPU* cpu, uchar *program, size_t program_length);

#endif // CPU_H_
