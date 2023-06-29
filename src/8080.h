#ifndef CPU8080_H
#define CPU8080_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 64000

#define DEBUG

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(uint8_t)  \
  (uint8_t & 0x80 ? '1' : '0'), \
  (uint8_t & 0x40 ? '1' : '0'), \
  (uint8_t & 0x20 ? '1' : '0'), \
  (uint8_t & 0x10 ? '1' : '0'), \
  (uint8_t & 0x08 ? '1' : '0'), \
  (uint8_t & 0x04 ? '1' : '0'), \
  (uint8_t & 0x02 ? '1' : '0'), \
  (uint8_t & 0x01 ? '1' : '0') 

#define CARRY_FLAG_BIT 0
#define PARITY_FLAG_BIT 2
#define AUX_CARRY_FLAG_BIT 4
#define ZERO_FLAG_BIT 6
#define SIGN_FLAG_BIT 7

struct CPU
{
	uint16_t PC;
	uint16_t SP;

	uint8_t FLAGS;
	uint8_t REG_A;
	uint8_t REG_B;
	uint8_t REG_C;
	uint8_t REG_D;
	uint8_t REG_E;
	uint8_t REG_H;
	uint8_t REG_L;

	bool HLT;
	bool INT;

	uint8_t IO_IN[256];
	uint8_t IO_OUT[256];

	uint64_t CYCLES;

	uint8_t MEM[MEMORY_SIZE];
}CPU;


bool test_finished;

const uint8_t instruction_cycles_table[256];

const uint8_t instruction_cycles_table_secondary[256];

void PrintCPUState();

void InitCPU();

void Interrupt();

// Changes n bit to value indicated by parameter value. n = 0 is the least significant bit
uint8_t ChangeNBit(uint8_t num, uint8_t n, bool value);

// Carry for addition value_1 + value_2 (+ carry)
void CarryFlag(uint8_t value_1, uint8_t value_2, bool carry);

void ParityFlag(uint8_t result);

void AuxCarryFlag(uint8_t value_1, uint8_t value_2, bool carry);

void ZeroFlag(uint8_t result);

void SignFlag(uint8_t result);

void ZeroSignParity(uint8_t value);

bool GetCarryFlag();

bool GetParityFlag();

bool GetAuxCarryFlag();

bool GetZeroFlag();

bool GetSignFlag();

void ClearCarryFlag();

void ClearParityFlag();

void ClearAuxCarryFlag();

void ClearZeroFlag();

void ClearSignFlag();

void SetCarryFlag();

void SetParityFlag();

void SetAuxCarryFlag();

void SetZeroFlag();

void SetSignFlag();

// Combine register pair into one 16-bit value
uint16_t CombineRP(uint8_t high_order_byte, uint8_t low_order_byte);

// Store 16-bit value between two 8-bit registers
void StoreRP(uint8_t* high_order_register, uint8_t* low_order_register, uint16_t number);

void ADD(uint8_t value_1, uint8_t value_2, bool carry);

void SUB(uint8_t value_1, uint8_t value_2, bool carry);

uint8_t INR(uint8_t value);

uint8_t DCR(uint8_t value);

void ANA(uint8_t value);

void XRA(uint8_t value);

void ORA(uint8_t value);

void CMP(uint8_t value_1, uint8_t value_2);

void RET();

void JMP();

void CALL();

void IO_OUT();

void IO_IN();


void Execute();

void LoadProgram();

void Run();

#endif // !CPU8080_H
