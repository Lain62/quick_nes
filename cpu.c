#include "cpu.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define uchar unsigned char
#define ushort unsigned short
#define MEMORY_SIZE 0xFFFF

CPU make_cpu(void)
{
    return (CPU){
	.reg_a = 0,
	.status = 0,
	.pc = 0,
	.reg_x = 0,
	.reg_y = 0,
	.memory = {}
    };
}

INSTRUCTION_SET cpu_turn_op_into_instruction_set(uchar op)
{
    // (op_code, instruction, bytes, cycles, mode)
    switch (op) {
    // BRK
    instruction_case(0x00, INSTRUCTION_BRK, 1, 7, ADDRESS_NONE);
    // ADC
    instruction_case(0x69, INSTRUCTION_ADC, 2, 2, ADDRESS_IMMEDIATE);
    instruction_case(0x65, INSTRUCTION_ADC, 2, 3, ADDRESS_IMMEDIATE);
    instruction_case(0x75, INSTRUCTION_ADC, 2, 4, ADDRESS_IMMEDIATE);
    instruction_case(0x6D, INSTRUCTION_ADC, 3, 4, ADDRESS_IMMEDIATE);
    instruction_case(0x7D, INSTRUCTION_ADC, 3, 4, ADDRESS_IMMEDIATE);  // cycle +1 if page is crossed
    instruction_case(0x79, INSTRUCTION_ADC, 3, 4, ADDRESS_IMMEDIATE);  // cycle +1 if page is crossed
    instruction_case(0x61, INSTRUCTION_ADC, 2, 6, ADDRESS_IMMEDIATE);
    instruction_case(0x71, INSTRUCTION_ADC, 2, 5, ADDRESS_IMMEDIATE);  // cycle +1 if page is crossed  
    // AND
    instruction_case(0x29, INSTRUCTION_AND, 2, 2, ADDRESS_IMMEDIATE);
    instruction_case(0x25, INSTRUCTION_AND, 2, 3, ADDRESS_ZEROPAGE);
    instruction_case(0x35, INSTRUCTION_AND, 2, 4, ADDRESS_ZEROPAGE_X);
    instruction_case(0x2D, INSTRUCTION_AND, 3, 4, ADDRESS_ABSOLUTE);
    instruction_case(0x3D, INSTRUCTION_AND, 3, 4, ADDRESS_ABSOLUTE_X); // cycle +1 if page is crossed
    instruction_case(0x39, INSTRUCTION_AND, 3, 4, ADDRESS_ABSOLUTE_Y); // cycle +1 if page is crossed
    instruction_case(0x21, INSTRUCTION_AND, 2, 6, ADDRESS_INDIRECT_X);
    instruction_case(0x31, INSTRUCTION_AND, 2, 5, ADDRESS_INDIRECT_Y);    
    // LDA
    instruction_case(0xA9, INSTRUCTION_LDA, 2, 2, ADDRESS_IMMEDIATE);
    instruction_case(0xA5, INSTRUCTION_LDA, 2, 3, ADDRESS_ZEROPAGE);
    instruction_case(0xB5, INSTRUCTION_LDA, 2, 4, ADDRESS_ZEROPAGE_X);
    instruction_case(0xAD, INSTRUCTION_LDA, 3, 4, ADDRESS_ABSOLUTE);
    instruction_case(0xBD, INSTRUCTION_LDA, 3, 4, ADDRESS_ABSOLUTE_X); // cycle +1 if page is crossed
    instruction_case(0xB9, INSTRUCTION_LDA, 3, 4, ADDRESS_ABSOLUTE_Y); // cycle +1 if page is crossed
    instruction_case(0xA1, INSTRUCTION_LDA, 2, 6, ADDRESS_INDIRECT_X);
    instruction_case(0xB1, INSTRUCTION_LDA, 2, 5, ADDRESS_INDIRECT_Y); // cycle +1 if page is crossed
    // TAX
    instruction_case(0xAA, INSTRUCTION_TAX, 1, 2, ADDRESS_NONE);
    // INX
    instruction_case(0xE8, INSTRUCTION_INX, 1, 2, ADDRESS_NONE);
    // INY
    instruction_case(0xC8, INSTRUCTION_INY, 1, 2, ADDRESS_NONE);
    default: {
	assert("ERROR: OP CODE NOT IMPLEMENTED");
	return (INSTRUCTION_SET){};
    } break;
    };
    return (INSTRUCTION_SET){};
}

uchar cpu_read_memory(CPU* cpu, ushort addr)
{
    return cpu->memory[addr];
}

void cpu_write_memory(CPU* cpu, ushort addr, uchar data)
{
    cpu->memory[addr] = data;
}

ushort cpu_read_memory_ushort(CPU* cpu, ushort pos)
{
    ushort lo = (ushort)cpu_read_memory(cpu, pos);
    ushort hi = (ushort)cpu_read_memory(cpu, pos + 1);
    return (hi << 8) | (ushort)lo;
}

void cpu_write_memory_ushort(CPU* cpu, ushort pos, ushort data)
{
    uchar hi = (uchar)(data >> 8);
    uchar lo = (uchar)(data & 0xff);
    cpu_write_memory(cpu, pos, lo);
    cpu_write_memory(cpu, pos + 1, hi);
}

void cpu_reset(CPU* cpu)
{
    cpu->reg_a = 0;
    cpu->reg_x = 0;
    cpu->reg_y = 0;
    cpu->status = 0;

    cpu->pc = cpu_read_memory_ushort(cpu, 0xFFFC);
}

void cpu_load(CPU* cpu, uchar *program, size_t program_length)
{
    if (program_length + 0x8000 > MEMORY_SIZE) return;
    memcpy(&cpu->memory[0x8000], program, program_length);    
    cpu_write_memory_ushort(cpu, 0xFFFC, 0x8000);
}

ushort cpu_get_operand_address(CPU* cpu, ADDRESS_MODE mode)
{
    switch(mode) {
    case ADDRESS_IMMEDIATE: {
	return cpu->pc;
    } break;
    case ADDRESS_ZEROPAGE: {
	return (ushort)cpu_read_memory(cpu, cpu->pc);
    } break;
    case ADDRESS_ABSOLUTE: {
	return cpu_read_memory_ushort(cpu, cpu->pc);
    } break;
    case ADDRESS_ZEROPAGE_X: {
	uchar pos = cpu_read_memory(cpu, cpu->pc);
	ushort addr = (uchar)(pos + cpu->reg_x);
	return addr;
    } break;
    case ADDRESS_ZEROPAGE_Y: {
	uchar pos = cpu_read_memory(cpu, cpu->pc);
	ushort addr = (uchar)(pos + cpu->reg_y);
	return addr;
    } break;
    case ADDRESS_ABSOLUTE_X: {
	uchar base = cpu_read_memory_ushort(cpu, cpu->pc);
	ushort addr = (uchar)(base + (ushort)cpu->reg_x);
	return addr;
    } break;
    case ADDRESS_ABSOLUTE_Y: {
	uchar base = cpu_read_memory_ushort(cpu, cpu->pc);
	ushort addr = (uchar)(base + (ushort)cpu->reg_y);
	return addr;
    } break;
    case ADDRESS_INDIRECT_X: {
	uchar base = cpu_read_memory(cpu, cpu->pc);
	uchar ptr = (uchar)(base + cpu->reg_x);
	uchar lo = cpu_read_memory(cpu, (ushort)ptr);
	uchar hi = cpu_read_memory(cpu, (ushort)((uchar)(ptr + 1)));
	return ((ushort)hi << 8) | (ushort)lo;
    } break;
    case ADDRESS_INDIRECT_Y: {
	uchar base = cpu_read_memory(cpu, cpu->pc);
	uchar lo = cpu_read_memory(cpu, (ushort)base);
	uchar hi = cpu_read_memory(cpu, (ushort)((uchar)(base + 1)));
	ushort deref_base = ((ushort)hi << 8) | (ushort)lo;
	ushort deref = (ushort)(deref_base + cpu->reg_y);
	return deref;
    } break;
    default: {
    } break;
    }
    assert("MODE NOT SUPPORTED");
    return 1;
}

void cpu_set_overflow_flag(CPU* cpu)
{
    cpu->status = cpu->status | 0b01000000;    
}

void cpu_remove_overflow_flag(CPU* cpu)
{
    cpu->status = cpu->status & 0b10111111;        
}

void cpu_set_carry_flag(CPU* cpu)
{
    cpu->status = cpu->status | 0b00000001;
}

void cpu_remove_carry_flag(CPU* cpu)
{
    cpu->status = cpu->status & 0b11111110;
}

void cpu_update_zero_and_negative_flags(CPU* cpu, uchar result)
{
    if (result == 0) {
	cpu->status = cpu->status | 0b00000010; 
    } else {
	cpu->status = cpu->status & 0b11111101; 
    };

    if ((result & 0b10000000) != 0) { 
	cpu->status = cpu->status | 0b10000000; 
    } else {
	cpu->status = cpu->status & 0b01111111; 
    }
}

void cpu_instruction_LDA(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    uchar value = cpu_read_memory(cpu, addr);
    cpu->reg_a = value;
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_a);
}

void cpu_instruction_TAX(CPU* cpu)
{
    cpu->reg_x = cpu->reg_a;
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_x);
}

void cpu_instruction_INX(CPU* cpu)
{
    cpu->reg_x = cpu->reg_x + 1;
    
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_x);
}

void cpu_instruction_INY(CPU* cpu)
{
    cpu->reg_y = cpu->reg_y + 1;
    
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_y);
}

void cpu_instruction_STA(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    cpu_write_memory(cpu, addr, cpu->reg_a);
}

void cpu_instruction_AND(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    ushort result = cpu->reg_a & cpu_read_memory(cpu, addr);
    cpu->reg_a = result;
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_a);
}

// TODO: check if this is right, cuz im not sure if this is how its supposed to behave
void cpu_instruction_ADC(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    ushort result = cpu->reg_a + cpu_read_memory(cpu, addr);
    
    if ((cpu->status & 0b00000001) != 0) {
	result = result + 1;
    }
    if (result > 255) {
	cpu_set_carry_flag(cpu);
	result = result - 256;
    } else {
	cpu_remove_carry_flag(cpu);	
    }

    if (result > 127) {
	cpu_set_overflow_flag(cpu);
    } else {
	cpu_remove_overflow_flag(cpu);
    }
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_a);
    cpu->reg_a = (ushort)result;
}

void cpu_run(CPU* cpu)
{
    while (1) {
	INSTRUCTION_SET instruction_set = cpu_turn_op_into_instruction_set(cpu_read_memory(cpu, cpu->pc));
	cpu->pc = cpu->pc + 1;

	switch (instruction_set.instruction) {
	case INSTRUCTION_ADC: {
	    cpu_instruction_ADC(cpu, instruction_set.mode);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;	    
	} break;
	case INSTRUCTION_AND: {
	    cpu_instruction_AND(cpu, instruction_set.mode);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	case INSTRUCTION_BRK: {
	    return;
	} break;
	case INSTRUCTION_LDA: {	    
	    cpu_instruction_LDA(cpu, instruction_set.mode);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	case INSTRUCTION_TAX: {	    
	    cpu_instruction_TAX(cpu);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	case INSTRUCTION_INX: {	    
	    cpu_instruction_INX(cpu);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	case INSTRUCTION_INY: {	    
	    cpu_instruction_INY(cpu);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	}
    }
}

void cpu_load_and_run(CPU* cpu, uchar *program, size_t program_length)
{
    cpu_load(cpu, program, program_length);
    cpu_reset(cpu);
    cpu_run(cpu);
}
