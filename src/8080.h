#include <stdint.h>
#include <stdbool.h>
typedef uint8_t byte;

struct cpu
{
	uint16_t PC;
	uint16_t SP;

	byte FLAGS;
	byte REG_A;
	byte REG_B;
	byte REG_C;
	byte REG_D;
	byte REG_E;
	byte REG_H;
	byte REG_L;

	bool HLT;
	bool INT;

	byte IO_IN[256];
	byte IO_OUT[256];

	byte MEM[64000];
}CPU;

const byte instruction_cycles_table[256];

const byte instruction_cycles_table_secondary[256];

void InitCPU();

void Execute();

void Run();

