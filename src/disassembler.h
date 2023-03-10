#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
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

typedef uint8_t byte;

const char* instruction_table[256];

int LoadFile(const char* file_name, byte** file_data, uint16_t* file_size);

uint16_t GetCodeLine(byte* object_file, byte instruction_byte, uint16_t* PC, char* code_line);

int Disassemble();

#endif // !DISASSEMBLER_H