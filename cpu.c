#include "cpu.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define uchar unsigned char
#define ushort unsigned short
#define MEMORY_SIZE 0xFFFF

CPU make_cpu(void)
{
    return (CPU){
	.reg_a = 0,
	.status = 0b00100000,
	.pc = 0,
	.reg_x = 0,
	.reg_y = 0,
	.memory = {}
    };
}

ushort add_wrap_ushort(ushort a, ushort b)
{
	if (((int)a + (int)b) > USHRT_MAX) {
		int res = (int)a + (int)b;
		res = res - USHRT_MAX - 1;
		return (ushort)res;
	} else {
		return a + b;
	}
}

INSTRUCTION_SET cpu_turn_op_into_instruction_set(uchar op)
{
    // (op_code, instruction, bytes, cycles, mode)
    switch (op) {
    // BRK
    instruction_case(0x00, INSTRUCTION_BRK, 1, 7, ADDRESS_NONE);
	// CLEAR
    instruction_case(0x18, INSTRUCTION_CLC, 1, 2, ADDRESS_ZEROPAGE);
	instruction_case(0xD8, INSTRUCTION_CLD, 1, 2, ADDRESS_ZEROPAGE);
	instruction_case(0x58, INSTRUCTION_CLI, 1, 2, ADDRESS_ZEROPAGE);
	instruction_case(0xB8, INSTRUCTION_CLV, 1, 2, ADDRESS_ZEROPAGE);	
	// BIT
    instruction_case(0x24, INSTRUCTION_BIT, 2, 3, ADDRESS_ZEROPAGE);
	instruction_case(0x2C, INSTRUCTION_BIT, 3, 4, ADDRESS_ABSOLUTE);	
	// BRANCHES
	// TODO: Test all these branches????????
    instruction_case(0x90, INSTRUCTION_BCC, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
	instruction_case(0xB0, INSTRUCTION_BCS, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
	instruction_case(0xF0, INSTRUCTION_BEQ, 2, 2, ADDRESS_RELATIVE); //	+1 if branch succeeds +2 if to a new page
	instruction_case(0x30, INSTRUCTION_BMI, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
	instruction_case(0xD0, INSTRUCTION_BNE, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
	instruction_case(0x10, INSTRUCTION_BPL, 2, 2, ADDRESS_RELATIVE); //	+1 if branch succeeds +2 if to a new page
	instruction_case(0x50, INSTRUCTION_BVC, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
	instruction_case(0x70, INSTRUCTION_BVS, 2, 2, ADDRESS_RELATIVE); // +1 if branch succeeds +2 if to a new page
    // ASL
    instruction_case(0x0A, INSTRUCTION_ASL, 1, 2, ADDRESS_ACCUMULATOR);
    instruction_case(0x06, INSTRUCTION_ASL, 2, 5, ADDRESS_ZEROPAGE);
    instruction_case(0x16, INSTRUCTION_ASL, 2, 6, ADDRESS_ZEROPAGE_X);
    instruction_case(0x0E, INSTRUCTION_ASL, 3, 6, ADDRESS_ABSOLUTE);
    instruction_case(0x1E, INSTRUCTION_ASL, 3, 7, ADDRESS_ABSOLUTE_X);
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
    cpu->status = 0b00100000;

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

bool cpu_contains_flag(CPU* cpu, CPU_FLAG flag)
{
	switch (flag) {
	case FLAG_OVERFLOW: {
		if ((cpu->status & 0b01000000) == 0) return false;
    	return true;
	} break;
	case FLAG_CARRY: {
		if ((cpu->status & 0b00000001) == 0) return false;
		return true;
	} break;
	case FLAG_ZERO: {
		if ((cpu->status & 0b00000010) == 0) return false;
		return true;
	} break;
	case FLAG_NEGATIVE: {
		if ((cpu->status & 0b10000000) == 0) return false;
		return true;
	} break;
	default: {
		assert("TODO");
	} break;
    }
	return false;
}

void cpu_add_flag(CPU* cpu, CPU_FLAG flag)
{
	switch (flag) {
	case FLAG_OVERFLOW: {
		cpu->status = cpu->status | 0b01000000;    		
	} break;
	case FLAG_CARRY: {
		cpu->status = cpu->status | 0b00000001;		
	} break;
	case FLAG_ZERO: {
		cpu->status = cpu->status | 0b00000010;
	} break;
	case FLAG_NEGATIVE: {
		cpu->status = cpu->status | 0b10000000;
	} break;
	default: {
		assert("TODO");
	} break;
    }
	
}

void cpu_remove_flag(CPU* cpu, CPU_FLAG flag)
{
	switch (flag) {
	case FLAG_OVERFLOW: {
		cpu->status = cpu->status & 0b10111111;
	} break;
	case FLAG_CARRY: {
		cpu->status = cpu->status & 0b11111110;		
	} break;
	case FLAG_ZERO: {
		cpu->status = cpu->status & 0b11111101;
	} break;
	case FLAG_NEGATIVE: {
		cpu->status = cpu->status & 0b01111111; 
	} break;
	default: {
		assert("TODO");
	} break;
    }
	
}

void cpu_update_zero_and_negative_flags(CPU* cpu, uchar result)
{
    if (result == 0) {
		cpu_add_flag(cpu, FLAG_ZERO);
    } else {
		cpu_remove_flag(cpu, FLAG_ZERO);
    };

    if ((result & 0b10000000) != 0) { 
		cpu_add_flag(cpu, FLAG_NEGATIVE);
    } else {
		cpu_remove_flag(cpu, FLAG_NEGATIVE);
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

void cpu_instruction_ADC(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr = cpu_get_operand_address(cpu, mode);
    ushort result = cpu->reg_a + cpu_read_memory(cpu, addr);
    if ((cpu->status & 0b00000001) != 0) result = result + 1;
	
    if (result > 0xFF) {
	    cpu_add_flag(cpu, FLAG_CARRY);
	} else {
		cpu_remove_flag(cpu, FLAG_CARRY);
    }
	
	if ((uchar)result > 0x80) {
		cpu_add_flag(cpu, FLAG_OVERFLOW);
	} else {		
		cpu_remove_flag(cpu, FLAG_OVERFLOW);
	}
   
    cpu_update_zero_and_negative_flags(cpu, cpu->reg_a);
    cpu->reg_a = (uchar)result;
}

void cpu_instruction_ASL(CPU* cpu, ADDRESS_MODE mode)
{
    ushort addr;
    ushort value;
    if (mode != ADDRESS_ACCUMULATOR) {
	addr = cpu_get_operand_address(cpu, mode);
	value = cpu_read_memory(cpu, addr);
    } else {
	value = cpu->reg_a;
    }

    if ((value & 0b10000000) != 0) {
		cpu_add_flag(cpu, FLAG_CARRY);
		value = (value & 0b01111111);
		value = value << 1;
    } else {
		cpu_remove_flag(cpu, FLAG_CARRY);
		value = value << 1;
    }
    cpu_update_zero_and_negative_flags(cpu, value);

    if (mode != ADDRESS_ACCUMULATOR) {
		ushort addr = cpu_get_operand_address(cpu, mode);
		cpu_write_memory(cpu, addr, value);
    } else {
		cpu->reg_a = value;
    }    
}

void cpu_instruction_branch(CPU* cpu, bool condition)
{
	if (condition) {
		char jmp = cpu_read_memory(cpu, cpu->pc);
		ushort jmp_addr = cpu->pc;
		jmp_addr = add_wrap_ushort(jmp_addr, 1);
		jmp_addr = add_wrap_ushort(jmp_addr, (ushort)jmp);

		cpu->pc = jmp_addr;
	}
}

void cpu_instruction_BIT(CPU* cpu, ADDRESS_MODE mode)
{
	ushort addr = cpu_get_operand_address(cpu, mode);
	if ((cpu->reg_a & cpu_read_memory(cpu, addr)) == 0) {
		cpu_add_flag(cpu, FLAG_ZERO);
	} else {
		cpu_remove_flag(cpu, FLAG_ZERO);
	}
	if ((0b10000000 & cpu_read_memory(cpu, addr)) != 0) {
		cpu_add_flag(cpu, FLAG_NEGATIVE);
	} else {
		cpu_remove_flag(cpu, FLAG_NEGATIVE);
	}
	if ((0b01000000 & cpu_read_memory(cpu, addr)) != 0) {
		cpu_add_flag(cpu, FLAG_OVERFLOW);
	} else {
		cpu_remove_flag(cpu, FLAG_OVERFLOW);
	}	
}

void cpu_run(CPU* cpu)
{
    while (1) {
	INSTRUCTION_SET instruction_set = cpu_turn_op_into_instruction_set(cpu_read_memory(cpu, cpu->pc));
	cpu->pc = cpu->pc + 1;

	switch (instruction_set.instruction) {
	case INSTRUCTION_ASL: {
	    cpu_instruction_ASL(cpu, instruction_set.mode);
	    cpu->pc = cpu->pc + instruction_set.bytes - 1;	    
	} break;
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
	case INSTRUCTION_BCC: {
		cpu_instruction_branch(cpu, !cpu_contains_flag(cpu, FLAG_CARRY));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;
	} break;
	case INSTRUCTION_BCS: {
		cpu_instruction_branch(cpu, cpu_contains_flag(cpu, FLAG_CARRY));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;		
	} break;
	case INSTRUCTION_BEQ: {
		cpu_instruction_branch(cpu, cpu_contains_flag(cpu, FLAG_ZERO));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;		
	} break;
	case INSTRUCTION_BMI: {
		cpu_instruction_branch(cpu, cpu_contains_flag(cpu, FLAG_NEGATIVE));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_BNE: {
		cpu_instruction_branch(cpu, !cpu_contains_flag(cpu, FLAG_ZERO));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_BPL: {
		cpu_instruction_branch(cpu, !cpu_contains_flag(cpu, FLAG_NEGATIVE));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_BVC: {
		cpu_instruction_branch(cpu, !cpu_contains_flag(cpu, FLAG_OVERFLOW));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_BVS: {
		cpu_instruction_branch(cpu, cpu_contains_flag(cpu, FLAG_OVERFLOW));
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_BIT: {
		cpu_instruction_BIT(cpu, instruction_set.mode);
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_CLC: {
		cpu_remove_flag(cpu, FLAG_CARRY);
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_CLD: {
		cpu_remove_flag(cpu, FLAG_DECIMAL);
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_CLI: {
		cpu_remove_flag(cpu, FLAG_INTERRUPT_DISABLE);
		cpu->pc = cpu->pc + instruction_set.bytes - 1;				
	} break;
	case INSTRUCTION_CLV: {
		cpu_remove_flag(cpu, FLAG_OVERFLOW);
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
