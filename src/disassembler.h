#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdint.h>
// 111 = A
// 000 = B
// 001 = C
// 010 = D
// 011 = E
// 100 = H
// 101 = L

// rp
// B = BC
// D = DE
// H = HL

// RP
// 00 = BC
// 01 = DE
// 10 = HL
// 11 = SP

// rh first register (high order)
// rl second register (low order)

// FLAGS  bit 7: sign, bit 6: zero, bit 5: NULL, bit 4: ac, bit 3: NULL, bit 2: parity, bit 1: NULL, bit 0: carry

#define DISASSEMBLY_LINE_PREFIX_LENGTH 23
#define CODE_LINE_LENGTH 14

const char* instruction_table[256];

int LoadFile(const char* file_name, uint8_t** file_data, uint16_t* file_size);

uint8_t GetDisassemblyLine(uint8_t* MEM, uint16_t PC, char* print_line);

uint8_t GetCodeLine(uint8_t* object_file, uint16_t* PC, char* code_line);

int Disassemble();

#endif // !DISASSEMBLER_H