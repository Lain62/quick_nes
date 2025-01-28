#include "cpu.h"
#include <assert.h>
#include <stdio.h>

void test_0xa9_lda_immediate_load_data()
{
    CPU cpu = make_cpu();
    uchar program[3] = {0xa9, 0x05, 0x00};
    cpu_load_and_run(&cpu, program, 3);
    assert(cpu.reg_a == 0x05);
    assert((cpu.status & 0b00000010) == 0b00);
    assert((cpu.status & 0b10000000) == 0);
    printf("PASSED: test_0xa9_lda_immediate_load_data\n");   
}

void test_0xa9_lda_zero_flag()
{
    CPU cpu2 = make_cpu();
    uchar program2[3] = {0xa9, 0x00, 0x00};
    cpu_load_and_run(&cpu2, program2, 3);
    assert((cpu2.status & 0b00000010) == 0b10);
    printf("PASSED: test_0xa9_lda_zero_flag\n");   
}

void test_5ops_working_together()
{
    CPU cpu = make_cpu();
    uchar program[5] = {0xA9, 0xC0, 0xAA, 0xE8, 0x00};
    cpu_load_and_run(&cpu, program, 5);    
    assert(cpu.reg_x == 0xC1);
    printf("PASSED: test_5ops_working_together\n");
}

void test_lda_from_memory() {
    CPU cpu = make_cpu();
    cpu_write_memory(&cpu, 0x10, 0x55);
    uchar program[3] = {0xA5, 0x10, 0x00};
    cpu_load_and_run(&cpu, program, 3);
    assert(cpu.reg_a == 0x55);
    printf("PASSED: test_lda_from_memory\n");
}

int main(void)
{
    test_0xa9_lda_immediate_load_data();
    test_0xa9_lda_zero_flag();
    test_5ops_working_together();
    test_lda_from_memory();
    return 0;
}

