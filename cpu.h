#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>

#define uchar unsigned char
#define ushort unsigned short
#define MEMORY_SIZE 0xFFFF

#define instruction_case(op, ins, by, cy, mo) \
       case op: { \
           return (INSTRUCTION_SET){ \
	       .op_code = op, \
	       .instruction = ins, \
	       .bytes = by, \
	       .cycles = cy, \
	       .mode = mo \
	   }; \
       } break \

typedef enum {
    INSTRUCTION_BRK,
    INSTRUCTION_LDA,
    INSTRUCTION_TAX,
    INSTRUCTION_INX,
    INSTRUCTION_INY,
    INSTRUCTION_AND,
    INSTRUCTION_ADC,
    INSTRUCTION_ASL
} INSTRUCTION;

typedef enum {
    ADDRESS_ACCUMULATOR,
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

typedef enum {
	FLAG_NEGATIVE,
	FLAG_OVERFLOW,
	FLAG_B,
	FLAG_DECIMAL,
	FLAG_INTERRUPT_DISABLE,
	FLAG_ZERO,
	FLAG_CARRY
} CPU_FLAG;


typedef struct INSTRUCTION_SET{
    uchar op_code;
    INSTRUCTION instruction;
    uchar bytes;
    uchar cycles;
    ADDRESS_MODE mode;    
} INSTRUCTION_SET;

typedef struct CPU{
    uchar reg_a;
    uchar reg_x;
    uchar reg_y;
    uchar status;
    ushort pc;
    uchar memory[MEMORY_SIZE];
} CPU;

CPU make_cpu(void);

uchar cpu_read_memory(CPU* cpu, ushort addr);

void cpu_write_memory(CPU* cpu, ushort addr, uchar data);

ushort cpu_read_memory_ushort(CPU* cpu, ushort pos);

void cpu_write_memory_ushort(CPU* cpu, ushort pos, ushort data);

void cpu_reset(CPU* cpu);

void cpu_load(CPU* cpu, uchar *program, size_t program_length);

ushort cpu_get_operand_address(CPU* cpu, ADDRESS_MODE mode);

void cpu_set_flag(CPU* cpu, CPU_FLAG flag);

void cpu_remove_flag(CPU* cpu, CPU_FLAG flag);

void cpu_update_zero_and_negative_flags(CPU* cpu, uchar result);

void cpu_instruction_LDA(CPU* cpu, ADDRESS_MODE mode);

void cpu_instruction_TAX(CPU* cpu);

void cpu_instruction_INX(CPU* cpu);

void cpu_instruction_INY(CPU* cpu);

void cpu_instruction_STA(CPU* cpu, ADDRESS_MODE mode);

void cpu_instruction_AND(CPU* cpu, ADDRESS_MODE mode);

void cpu_instruction_ADC(CPU* cpu, ADDRESS_MODE mode);

void cpu_instruction_ASL(CPU* cpu, ADDRESS_MODE mode);

void cpu_run(CPU* cpu);

void cpu_load_and_run(CPU* cpu, uchar *program, size_t program_length);

#endif // CPU_H_
