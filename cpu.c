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
	assert("MODE NOT SUPPORTED");
    } break;
    }
    assert("MODE NOT SUPPORTED");
    return 1;
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

void cpu_instruction_STA(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    cpu_write_memory(cpu, addr, cpu->reg_a);
}

void cpu_run(CPU* cpu)
{
    while (1) {
	uchar opcode = cpu_read_memory(cpu, cpu->pc);	
	cpu->pc = cpu->pc + 1;

	switch (opcode) {
	case 0x85: {
	    cpu_instruction_STA(cpu, ADDRESS_ZEROPAGE);
	    cpu->pc = cpu->pc + 1;	    
	} break;
	case 0x95: {
	    cpu_instruction_STA(cpu, ADDRESS_ZEROPAGE_X);
	    cpu->pc = cpu->pc + 1;	    
	} break;
	case 0xA9: {
	    cpu_instruction_LDA(cpu, ADDRESS_IMMEDIATE);
	    cpu->pc = cpu->pc + 1;
	} break;
	case 0xA5: {
	    cpu_instruction_LDA(cpu, ADDRESS_ZEROPAGE);
	    cpu->pc = cpu->pc + 1;	    
	} break;
	case 0xAD: {
	    cpu_instruction_LDA(cpu, ADDRESS_ABSOLUTE);
	    cpu->pc = cpu->pc + 1;	    
	} break;
	case 0xAA: {
	    cpu_instruction_TAX(cpu);
	} break;
	case 0xE8: {
	    cpu_instruction_INX(cpu);
	} break;
	case 0x00: {
	    return;
	} break;
	default: {
	    // TODO!
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
