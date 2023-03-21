#include "8080.h"
#include "disassembler.h"
#include "timer.h"
#include <stdlib.h>


const uint8_t instruction_cycles_table[256] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11,
	5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11,
	5, 10, 10, 18, 11, 11, 7, 11, 5, 5, 10, 5, 11, 17, 7, 11,
	5, 10, 10, 4, 11, 11, 7, 11, 5, 5, 10, 4, 11, 17, 7, 11
};

const uint8_t instruction_cycles_table_secondary[256] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 17, 17, 7, 11,
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 17, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11
};

void PrintCPUState()
{
	printf("\nPC: 0x%X", CPU.PC);
	printf("\nSP: 0x%X", CPU.SP);

	printf("\n       SZ_A_P_C\nFLAGS: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(CPU.FLAGS));

	printf("\nA: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_A, BYTE_TO_BINARY(CPU.REG_A), CPU.REG_A);
	printf("\nB: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_B, BYTE_TO_BINARY(CPU.REG_B), CPU.REG_B);
	printf("\nC: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_C, BYTE_TO_BINARY(CPU.REG_C), CPU.REG_C);
	printf("\nD: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_D, BYTE_TO_BINARY(CPU.REG_D), CPU.REG_D);
	printf("\nE: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_E, BYTE_TO_BINARY(CPU.REG_E), CPU.REG_E);
	printf("\nH: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_H, BYTE_TO_BINARY(CPU.REG_H), CPU.REG_H);
	printf("\nL: 0x%02X 0b"BYTE_TO_BINARY_PATTERN" (% u)", CPU.REG_L, BYTE_TO_BINARY(CPU.REG_L), CPU.REG_L);

	printf("\n\n");
}

void InitCPU()
{
	CPU.PC = 0x0000;
	CPU.SP = 0xFFFF;

	CPU.FLAGS = 0b00000000;
	CPU.REG_A = 0x00;
	CPU.REG_B = 0x00;
	CPU.REG_C = 0x00;
	CPU.REG_D = 0x00;
	CPU.REG_E = 0x00;
	CPU.REG_H = 0x00;
	CPU.REG_L = 0x00;

	CPU.HLT = 0;
	CPU.INT = 0;

	memset(CPU.MEM, 0, MEMORY_SIZE);
}

void Interrupt()
{

}

// Changes n bit to value indicated by parameter value. n = 0 is the least significant bit
uint8_t ChangeNBit(uint8_t num, uint8_t n, bool value)
{
	return num & ~(1 << n) | (value << n);
}

// Carry for addition value_1 + value_2 (+ carry)
void CarryFlag(uint8_t value_1, uint8_t value_2, bool carry)
{
	int16_t result = value_1 + value_2 + carry;
	int16_t c = result ^ value_1 ^ value_2;
	ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, c & (1 << 8));
}

void ParityFlag(uint8_t result)
{
	uint8_t set_bits = 0;
	// Count number of set (1) bits
	for (uint8_t i = 0; i < 8; i++)
	{
		set_bits += ((result >> i) & 1);
	}
	// Sets parity flag (1) if number of set (1) bits is odd, otherwise parity flag is cleared (0)
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, PARITY_FLAG_BIT, ((set_bits & 1) == 1));
}

void AuxCarryFlag(uint8_t value_1, uint8_t value_2, bool carry)
{
	int16_t result = value_1 + value_2 + carry;
	int16_t c = result ^ value_1 ^ value_2;
	ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, c & (1 << 4));
}

void ZeroFlag(uint8_t result)
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, ZERO_FLAG_BIT, (result == 0));
}

void SignFlag(uint8_t result)
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, SIGN_FLAG_BIT, (result & 0b10000000) >> 7);
}

void ZeroSignParity(uint8_t value)
{
	ZeroFlag(value);
	SignFlag(value);
	ParityFlag(value);
}

bool GetCarryFlag()
{
	return (CPU.FLAGS & 0b00000001) >> CARRY_FLAG_BIT;
}

bool GetParityFlag()
{
	return (CPU.FLAGS & 0b00000100) >> PARITY_FLAG_BIT;
}

bool GetAuxCarryFlag()
{
	return (CPU.FLAGS & 0b00010000) >> AUX_CARRY_FLAG_BIT;
}

bool GetZeroFlag()
{
	return (CPU.FLAGS & 0b01000000) >> ZERO_FLAG_BIT;
}

bool GetSignFlag()
{
	return (CPU.FLAGS & 0b10000000) >> SIGN_FLAG_BIT;
}


void ClearCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, 0);
}

void ClearParityFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, PARITY_FLAG_BIT, 0);
}

void ClearAuxCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, 0);
}

void ClearZeroFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, ZERO_FLAG_BIT, 0);
}

void ClearSignFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, SIGN_FLAG_BIT, 0);
}


void SetCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, 1);
}

void SetParityFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, PARITY_FLAG_BIT, 1);
}

void SetAuxCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, 1);
}

void SetZeroFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, ZERO_FLAG_BIT, 1);
}

void SetSignFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, SIGN_FLAG_BIT, 1);
}


// Combine register pair into one 16-bit value
uint16_t CombineRP(uint8_t high_order_byte, uint8_t low_order_byte)
{
	return (uint16_t)(high_order_byte << 8) | low_order_byte;
}

// Store 16-bit value between two 8-bit registers
void StoreRP(uint8_t* high_order_register, uint8_t* low_order_register, uint16_t number)
{
	*high_order_register = number >> 8;
	*low_order_register = number & 0x00FF;
}


void ADD(uint8_t value_1, uint8_t value_2, bool carry)
{
	CPU.REG_A = value_1 + value_2 + carry;
	CarryFlag(value_1, value_2, carry);
	AuxCarryFlag(value_1, value_2, carry);
	ZeroSignParity(CPU.REG_A);
}

void SUB(uint8_t value_1, uint8_t value_2, bool carry)
{
	ADD(value_1, ~value_2, !carry);
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, !GetCarryFlag());
}

uint8_t INR(uint8_t value)
{
	uint8_t result = value + 1;
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, (result & 0xF) == 0);
	ZeroSignParity(result);
	return result;
}

uint8_t DCR(uint8_t value)
{
	uint8_t result = value - 1;
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, !((result & 0xF) == 0xF));
	ZeroSignParity(result);
	return result;
}

void ANA(uint8_t value)
{
	uint8_t result = CPU.REG_A & value;
	ClearCarryFlag();
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, (((CPU.REG_A | value) & 0x08) != 0));
	ZeroSignParity(result);
	CPU.REG_A = result;
}

void XRA(uint8_t value)
{
	CPU.REG_A ^= value;
	ClearCarryFlag();
	ClearAuxCarryFlag();
	ZeroSignParity(CPU.REG_A);
}

void ORA(uint8_t value)
{
	CPU.REG_A |= value;
	ClearCarryFlag();
	ClearAuxCarryFlag();
	ZeroSignParity(CPU.REG_A);
}

void CMP(uint8_t value_1, uint8_t value_2)
{
	int16_t result = value_1 - value_2;
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, result >> 8);
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, AUX_CARRY_FLAG_BIT, ~(value_1 ^ result ^ value_2) & 0x10);
	ZeroSignParity(result & 0xFF);
}

void RET()
{
	uint8_t low_order_byte = CPU.MEM[CPU.SP];
	uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
	uint16_t addr = CombineRP(high_order_byte, low_order_byte);
	CPU.PC = addr - 1;
	CPU.SP += 1; // Increment SP by 1
}

void JMP()
{
	// Since program counter gets incremented at the end of the Execute function decrement target PC by 1 for all branch instructons
	// (less typing since most instructions dont manipulate PC directly)
	CPU.PC = ((uint16_t)(CPU.MEM[CPU.PC + 1]) | (uint16_t)(CPU.MEM[CPU.PC + 2] << 8)) - 1;
}

void CALL()
{
	uint16_t next_instruction_addr = CPU.PC + 3;
	uint16_t high_order_addr = --CPU.SP;
	uint16_t low_order_addr = --CPU.SP;
	StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
	// Since program counter gets incremented at the end of the Execute function decrement target PC by 1 for all branch instructons
	// (less typing since most instructions dont manipulate PC directly)
	CPU.PC = ((uint16_t)(CPU.MEM[CPU.PC + 1]) | CPU.MEM[CPU.PC + 2] << 8) - 1;
}

void IO_OUT()
{
	uint8_t port = CPU.MEM[++CPU.PC];
	CPU.IO_OUT[port] = CPU.REG_A;
}

void IO_IN()
{
	uint8_t port = CPU.MEM[++CPU.PC];
	CPU.REG_A = CPU.IO_IN[port];
}

void Execute()
{
	// Get next instruction
	uint8_t op_code = CPU.MEM[CPU.PC];
	switch (op_code) 
	{
		case 0x00:	//NOP
			break;
		case 0x01:	//LXI B,d16
			CPU.REG_C = CPU.MEM[++CPU.PC];
			CPU.REG_B = CPU.MEM[++CPU.PC];
			break;
		case 0x02:	//STAX B
			CPU.MEM[CombineRP(CPU.REG_B, CPU.REG_C)] = CPU.REG_A;
			break;
		case 0x03:	//INX B
		{
			uint16_t value = CombineRP(CPU.REG_B, CPU.REG_C);
			value += 1;
			StoreRP(&CPU.REG_B, &CPU.REG_C, value);
		}
			break;
		case 0x04:	//INR B
			CPU.REG_B = INR(CPU.REG_B);
			break;
		case 0x05:	//DCR B
			CPU.REG_B = DCR(CPU.REG_B);
			break;
		case 0x06:	//MVI B,d8
			CPU.REG_B = CPU.MEM[++CPU.PC];
			break;
		case 0x07:	//RLC
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, (CPU.REG_A >> 7));
			CPU.REG_A = (CPU.REG_A << 1) | GetCarryFlag();
			break;
		case 0x08:	//*NOP
			break;
		case 0x09:	//DAD B
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t BC = CombineRP(CPU.REG_B, CPU.REG_C);
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, ((HL + BC) >> 16) & 1);
			StoreRP(&CPU.REG_H, &CPU.REG_L, HL + BC);
		}
			break;
		case 0x0A:	//LDAX B
			CPU.REG_A = CPU.MEM[CombineRP(CPU.REG_B, CPU.REG_C)];
			break;
		case 0x0B:	//DCX B
		{
			uint16_t value = CombineRP(CPU.REG_B, CPU.REG_C);
			value -= 1;
			StoreRP(&CPU.REG_B, &CPU.REG_C, value);
		}
			break;
		case 0x0C:	//INR C
			CPU.REG_C = INR(CPU.REG_C);
			break;
		case 0x0D:	//DCR C
			CPU.REG_C = DCR(CPU.REG_C);
			break;
		case 0x0E:	//MVI C,d8
			CPU.REG_C = CPU.MEM[++CPU.PC];
			break;
		case 0x0F:	//RRC
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, (CPU.REG_A & 0x1));
			CPU.REG_A = (CPU.REG_A >> 1) | (GetCarryFlag() << 7);
			break;
		case 0x10:	//*NOP
			break;
		case 0x11:	//LXI D,d16
			CPU.REG_E = CPU.MEM[++CPU.PC];
			CPU.REG_D = CPU.MEM[++CPU.PC];
			break;
		case 0x12:	//STAX D
			CPU.MEM[CombineRP(CPU.REG_D, CPU.REG_E)] = CPU.REG_A;
			break;
		case 0x13:	//INX D
		{
			uint16_t value = CombineRP(CPU.REG_D, CPU.REG_E);
			value += 1;
			StoreRP(&CPU.REG_D, &CPU.REG_E, value);
		}
			break;
		case 0x14:	//INR D
			CPU.REG_D = INR(CPU.REG_D);
			break;
		case 0x15:	//DCR D
			CPU.REG_D = DCR(CPU.REG_D);
			break;
		case 0x16:	//MVI D,d8
			CPU.REG_D = CPU.MEM[++CPU.PC];
			break;
		case 0x17:	//RAL
		{
			uint8_t carry = GetCarryFlag(); // Get old carry flag
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, (CPU.REG_A >> 7)); // Update carry flag
			CPU.REG_A = ((CPU.REG_A << 1) | carry); // Shift left and set low order bit to old carry flag
		}
			break;
		case 0x18:	//*NOP
			break;
		case 0x19:	//DAD D
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t DE = CombineRP(CPU.REG_D, CPU.REG_E);
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, ((HL + DE) >> 16) & 1);
			StoreRP(&CPU.REG_H, &CPU.REG_L, HL + DE);
		}
			break;
		case 0x1A:	//LDAX D
			CPU.REG_A = CPU.MEM[CombineRP(CPU.REG_D, CPU.REG_E)];
			break;
		case 0x1B:	//DCX D
		{
			uint16_t value = CombineRP(CPU.REG_D, CPU.REG_E);
			value -= 1;
			StoreRP(&CPU.REG_D, &CPU.REG_E, value);
		}
			break;
		case 0x1C:	//INR E
			CPU.REG_E = INR(CPU.REG_E);
			break;
		case 0x1D:	//DCR E
			CPU.REG_E = DCR(CPU.REG_E);
			break;
		case 0x1E:	//MVI E,d8
			CPU.REG_E = CPU.MEM[++CPU.PC];
			break;
		case 0x1F:	//RAR
		{
			uint8_t carry = GetCarryFlag(); // Get old carry flag
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, (CPU.REG_A & 1)); // Update carry flag
			CPU.REG_A = ((CPU.REG_A >> 1) | (carry << 7)); // Shift right and set high order bit to old carry flag
		}
			break;
		case 0x20:	//*NOP
			break;
		case 0x21:	//LXI H,d16
			CPU.REG_L = CPU.MEM[++CPU.PC];
			CPU.REG_H = CPU.MEM[++CPU.PC];
			break;
		case 0x22:	//SHLD addr
		{
			uint8_t low_order_byte = CPU.MEM[++CPU.PC];
			uint8_t high_order_byte = CPU.MEM[++CPU.PC];
			uint16_t addr = CombineRP(high_order_byte, low_order_byte);
			CPU.MEM[addr] = CPU.REG_L;
			CPU.MEM[++addr] = CPU.REG_H;
		}
			break;
		case 0x23:	//INX H
		{
			uint16_t value = CombineRP(CPU.REG_H, CPU.REG_L);
			value += 1;
			StoreRP(&CPU.REG_H, &CPU.REG_L, value);
		}
			break;
		case 0x24:	//INR H
			CPU.REG_H = INR(CPU.REG_H);
			break;
		case 0x25:	//DCR H
			CPU.REG_H = DCR(CPU.REG_H);
			break;
		case 0x26:	//MVI H,d8
			CPU.REG_H = CPU.MEM[++CPU.PC];
			break;
		case 0x27:	//DAA
		{
			uint8_t carry = GetCarryFlag();
			uint8_t correction = 0;

			uint8_t low_4_bits = CPU.REG_A & 0x0F;
			uint8_t high_4_bits = CPU.REG_A >> 4;

			if (low_4_bits > 9 || GetAuxCarryFlag()) 
			{
				correction += 0x06;
			}

			if (GetCarryFlag() || high_4_bits > 9 || (high_4_bits >= 9 && low_4_bits > 9)) {
				correction += 0x60;
				carry = 1;
			}

			ADD(CPU.REG_A, correction, 0);
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, carry); // Set carry bit
		}
			break;
		case 0x28:	//*NOP
			break;
		case 0x29:	//DAD H
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, ((HL + HL) >> 16) & 1);
			StoreRP(&CPU.REG_H, &CPU.REG_L, HL + HL);
		}
			break;
		case 0x2A:	//LHLD addr
		{
			uint8_t low_order_byte = CPU.MEM[++CPU.PC];
			uint8_t high_order_byte = CPU.MEM[++CPU.PC];
			uint16_t addr = CombineRP(high_order_byte, low_order_byte);
			CPU.REG_L = CPU.MEM[addr];
			CPU.REG_H = CPU.MEM[++addr];
		}
			break;
		case 0x2B:	//DCX H
		{
			uint16_t temp = CombineRP(CPU.REG_H, CPU.REG_L);
			temp -= 1;
			StoreRP(&CPU.REG_H, &CPU.REG_L, temp);
		}
			break;
		case 0x2C:	//INR L
			CPU.REG_L = INR(CPU.REG_L);
			break;
		case 0x2D:	//DCR L
			CPU.REG_L = DCR(CPU.REG_L);
			break;
		case 0x2E:	//MVI L,d8
			CPU.REG_L = CPU.MEM[++CPU.PC];
			break;
		case 0x2F:	//CMA
			CPU.REG_A = ~CPU.REG_A;
			break;
		case 0x30:	//*NOP
			break;
		case 0x31:	//LXI SP,d16
		{
			uint8_t low_order_byte = CPU.MEM[++CPU.PC];
			uint8_t high_order_byte = CPU.MEM[++CPU.PC];
			CPU.SP = CombineRP(high_order_byte, low_order_byte);
		}
			break;
		case 0x32:	//STA addr
		{
			uint8_t low_order_byte = CPU.MEM[++CPU.PC];
			uint8_t high_order_byte = CPU.MEM[++CPU.PC];
			uint16_t addr = CombineRP(high_order_byte, low_order_byte);
			CPU.MEM[addr] = CPU.REG_A;
		}
			break;
		case 0x33:	//INX SP
			CPU.SP += 1;
			break;
		case 0x34:	//INR M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			uint8_t temp = CPU.MEM[addr];
			CPU.MEM[addr] = INR(temp);
		}
			break;
		case 0x35:	//DCR M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			uint8_t temp = CPU.MEM[addr];
			CPU.MEM[addr] = DCR(temp);
		}
			break;
		case 0x36:	//MVI M,d8
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.MEM[++CPU.PC];
			break;
		case 0x37:	// STC
			SetCarryFlag();
			break;
		case 0x38:	//*NOP
			break;
		case 0x39:	//DAD SP
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t SP = CPU.SP;
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, ((HL + SP) >> 16) & 1);
			StoreRP(&CPU.REG_H, &CPU.REG_L, HL + SP);
		}
			break;
		case 0x3A:	//LDA addr
		{
			uint8_t low_order_byte = CPU.MEM[++CPU.PC];
			uint8_t high_order_byte = CPU.MEM[++CPU.PC];
			uint16_t addr = CombineRP(high_order_byte, low_order_byte);
			CPU.REG_A = CPU.MEM[addr];
		}
			break;
		case 0x3B:	//DCX SP
			CPU.SP -= 1;
			break;
		case 0x3C:	//INR A
			CPU.REG_A = INR(CPU.REG_A);
			break;
		case 0x3D:	//DCR A
			CPU.REG_A = DCR(CPU.REG_A);
			break;
		case 0x3E:	//MVI A,d8
			CPU.REG_A = CPU.MEM[++CPU.PC];
			break;
		case 0x3F:	//CMC
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, CARRY_FLAG_BIT, !GetCarryFlag());
			break;
		case 0x40:	//MOV B,B
			CPU.REG_B = CPU.REG_B;
			break;
		case 0x41:	//MOV B,C
			CPU.REG_B = CPU.REG_C;
			break;
		case 0x42:	//MOV B,D
			CPU.REG_B = CPU.REG_D;
			break;
		case 0x43:	//MOV B,E
			CPU.REG_B = CPU.REG_E;
			break;
		case 0x44:	//MOV B,H
			CPU.REG_B = CPU.REG_H;
			break;
		case 0x45:	//MOV B,L
			CPU.REG_B = CPU.REG_L;
			break;
		case 0x46:	//MOV B,M
			CPU.REG_B = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x47:	//MOV B,A
			CPU.REG_B = CPU.REG_A;
			break;
		case 0x48:	//MOV C,B
			CPU.REG_C = CPU.REG_B;
			break;
		case 0x49:	//MOV C,C
			CPU.REG_C = CPU.REG_C;
			break;
		case 0x4A:	//MOV C,D
			CPU.REG_C = CPU.REG_D;
			break;
		case 0x4B:	//MOV C,E
			CPU.REG_C = CPU.REG_E;
			break;
		case 0x4C:	//MOV C,H
			CPU.REG_C = CPU.REG_H;
			break;
		case 0x4D:	//MOV C,L
			CPU.REG_C = CPU.REG_L;
			break;
		case 0x4E:	//MOV C,M
			CPU.REG_C = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x4F:	//MOV C,A
			CPU.REG_C = CPU.REG_A;
			break;
		case 0x50:	//MOV D,B
			CPU.REG_D = CPU.REG_B;
			break;
		case 0x51:	//MOV D,C
			CPU.REG_D = CPU.REG_C;
			break;
		case 0x52:	//MOV D,D
			CPU.REG_D = CPU.REG_D;
			break;
		case 0x53:	//MOV D,E
			CPU.REG_D = CPU.REG_E;
			break;
		case 0x54:	//MOV D,H
			CPU.REG_D = CPU.REG_H;
			break;
		case 0x55:	//MOV D,L
			CPU.REG_D = CPU.REG_L;
			break;
		case 0x56:	//MOV D,M
			CPU.REG_D = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x57:	//MOV D,A
			CPU.REG_D = CPU.REG_A;
			break;
		case 0x58:	//MOV E,B
			CPU.REG_E = CPU.REG_B;
			break;
		case 0x59:	//MOV E,C
			CPU.REG_E = CPU.REG_C;
			break;
		case 0x5A:	//MOV E,D
			CPU.REG_E = CPU.REG_D;
			break;
		case 0x5B:	//MOV E,E
			CPU.REG_E = CPU.REG_E;
			break;
		case 0x5C:	//MOV E,H
			CPU.REG_E = CPU.REG_H;
			break;
		case 0x5D:	//MOV E,L
			CPU.REG_E = CPU.REG_L;
			break;
		case 0x5E:	//MOV E,M
			CPU.REG_E = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x5F:	//MOV E,A
			CPU.REG_E = CPU.REG_A;
			break;
		case 0x60:	//MOV H,B
			CPU.REG_H = CPU.REG_B;
			break;
		case 0x61:	//MOV H,C
			CPU.REG_H = CPU.REG_C;
			break;
		case 0x62:	//MOV H,D
			CPU.REG_H = CPU.REG_D;
			break;
		case 0x63:	//MOV H,E
			CPU.REG_H = CPU.REG_E;
			break;
		case 0x64:	//MOV H,H
			CPU.REG_H = CPU.REG_H;
			break;
		case 0x65:	//MOV H,L
			CPU.REG_H = CPU.REG_L;
			break;
		case 0x66:	//MOV H,M
			CPU.REG_H = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x67:	//MOV H,A
			CPU.REG_H = CPU.REG_A;
			break;
		case 0x68:	//MOV L,B
			CPU.REG_L = CPU.REG_B;
			break;
		case 0x69:	//MOV L,C
			CPU.REG_L = CPU.REG_C;
			break;
		case 0x6A:	//MOV L,D
			CPU.REG_L = CPU.REG_D;
			break;
		case 0x6B:	//MOV L,E
			CPU.REG_L = CPU.REG_E;
			break;
		case 0x6C:	//MOV L,H
			CPU.REG_L = CPU.REG_H;
			break;
		case 0x6D:	//MOV L,L
			CPU.REG_L = CPU.REG_L;
			break;
		case 0x6E:	//MOV L,M
			CPU.REG_L = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x6F:	//MOV L,A
			CPU.REG_L = CPU.REG_A;
			break;
		case 0x70:	//MOV M,B
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_B;
			break;
		case 0x71:	//MOV M,C
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_C;
			break;
		case 0x72:	//MOV M,D
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_D;
			break;
		case 0x73:	//MOV M,E
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_E;
			break;
		case 0x74:	//MOV M,H
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_H;
			break;
		case 0x75:	//MOV M,L
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_L;
			break;
		case 0x76:	//HLT
			CPU.HLT = 1;
			break;
		case 0x77:	//MOV M,A
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] = CPU.REG_A;
			break;
		case 0x78:	//MOV A,B
			CPU.REG_A = CPU.REG_B;
			break;
		case 0x79:	//MOV A,C
			CPU.REG_A = CPU.REG_C;
			break;
		case 0x7A:	//MOV A,D
			CPU.REG_A = CPU.REG_D;
			break;
		case 0x7B:	//MOV A,E
			CPU.REG_A = CPU.REG_E;
			break;
		case 0x7C:	//MOV A,H
			CPU.REG_A = CPU.REG_H;
			break;
		case 0x7D:	//MOV A,L
			CPU.REG_A = CPU.REG_L;
			break;
		case 0x7E:	//MOV A,M
			CPU.REG_A = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			break;
		case 0x7F:	//MOV A,A
			CPU.REG_A = CPU.REG_A;
			break;
		case 0x80:	//ADD B
			ADD(CPU.REG_A, CPU.REG_B, 0);
			break;
		case 0x81:	//ADD C
			ADD(CPU.REG_A, CPU.REG_C, 0);
			break;
		case 0x82:	//ADD D
			ADD(CPU.REG_A, CPU.REG_D, 0);
			break;
		case 0x83:	//ADD E
			ADD(CPU.REG_A, CPU.REG_E, 0);
			break;
		case 0x84:	//ADD H
			ADD(CPU.REG_A, CPU.REG_H, 0);
			break;
		case 0x85:	//ADD L
			ADD(CPU.REG_A, CPU.REG_L, 0);
			break;
		case 0x86:	//ADD M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			ADD(CPU.REG_A, CPU.MEM[addr], 0);
		}
			break;
		case 0x87:	//ADD A
			ADD(CPU.REG_A, CPU.REG_A, 0);
			break;
		case 0x88:	//ADC B
			ADD(CPU.REG_A, CPU.REG_B, GetCarryFlag());
			break;
		case 0x89:	//ADC C
			ADD(CPU.REG_A, CPU.REG_C, GetCarryFlag());
			break;
		case 0x8A:	//ADC D
			ADD(CPU.REG_A, CPU.REG_D, GetCarryFlag());
			break;
		case 0x8B:	//ADC E
			ADD(CPU.REG_A, CPU.REG_E, GetCarryFlag());
			break;
		case 0x8C:	//ADC H
			ADD(CPU.REG_A, CPU.REG_H, GetCarryFlag());
			break;
		case 0x8D:	//ADC L
			ADD(CPU.REG_A, CPU.REG_L, GetCarryFlag());
			break;
		case 0x8E:	//ADC M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			ADD(CPU.REG_A, CPU.MEM[addr], GetCarryFlag());
		}
			break;
		case 0x8F:	//ADC A
			ADD(CPU.REG_A, CPU.REG_A, GetCarryFlag());
			break;
		case 0x90:	//SUB B
			SUB(CPU.REG_A, CPU.REG_B, 0);
			break;
		case 0x91:	//SUB C
			SUB(CPU.REG_A, CPU.REG_C, 0);
			break;
		case 0x92:	//SUB D
			SUB(CPU.REG_A, CPU.REG_D, 0);
			break;
		case 0x93:	//SUB E
			SUB(CPU.REG_A, CPU.REG_E, 0);
			break;
		case 0x94:	//SUB H
			SUB(CPU.REG_A, CPU.REG_H, 0);
			break;
		case 0x95:	//SUB L
			SUB(CPU.REG_A, CPU.REG_L, 0);
			break;
		case 0x96:	//SUB M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			SUB(CPU.REG_A, CPU.MEM[addr], 0);
		}
			break;
		case 0x97:	//SUB A
			SUB(CPU.REG_A, CPU.REG_A, 0);
			break;
		case 0x98:	//SBB B
			SUB(CPU.REG_A, CPU.REG_B, GetCarryFlag());
			break;
		case 0x99:	//SBB C
			SUB(CPU.REG_A, CPU.REG_C, GetCarryFlag());
			break;
		case 0x9A:	//SBB D
			SUB(CPU.REG_A, CPU.REG_D, GetCarryFlag());
			break;
		case 0x9B:	//SBB E
			SUB(CPU.REG_A, CPU.REG_E, GetCarryFlag());
			break;
		case 0x9C:	//SBB H
			SUB(CPU.REG_A, CPU.REG_H, GetCarryFlag());
			break;
		case 0x9D:	//SBB L
			SUB(CPU.REG_A, CPU.REG_L, GetCarryFlag());
			break;
		case 0x9E:	//SBB M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			SUB(CPU.REG_A, CPU.MEM[addr], GetCarryFlag());
		}
			break;
		case 0x9F:	//SBB A
			SUB(CPU.REG_A, CPU.REG_A, GetCarryFlag());
			break;
		case 0xA0:	//ANA B
			ANA(CPU.REG_B);
			break;
		case 0xA1:	//ANA C
			ANA(CPU.REG_C);
			break;
		case 0xA2:	//ANA D
			ANA(CPU.REG_D);
			break;
		case 0xA3:	//ANA E
			ANA(CPU.REG_E);
			break;
		case 0xA4:	//ANA H
			ANA(CPU.REG_H);
			break;
		case 0xA5:	//ANA L
			ANA(CPU.REG_L);
			break;
		case 0xA6:	//ANA M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			ANA(CPU.MEM[addr]);
		}
			break;
		case 0xA7:	//ANA A
			ANA(CPU.REG_A);
			break;
		case 0xA8:	//XRA B
			XRA(CPU.REG_B);
			break;
		case 0xA9:	//XRA C
			XRA(CPU.REG_C);
			break;
		case 0xAA:	//XRA D
			XRA(CPU.REG_D);
			break;
		case 0xAB:	//XRA E
			XRA(CPU.REG_E);
			break;
		case 0xAC:	//XRA H
			XRA(CPU.REG_H);
			break;
		case 0xAD:	//XRA L
			XRA(CPU.REG_L);
			break;
		case 0xAE:	//XRA M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			XRA(CPU.MEM[addr]);
		}
			break;
		case 0xAF:	//XRA A
			XRA(CPU.REG_A);
			break;
		case 0xB0:	//ORA B
			ORA(CPU.REG_B);
			break;
		case 0xB1:	//ORA C
			ORA(CPU.REG_C);
			break;
		case 0xB2:	//ORA D
			ORA(CPU.REG_D);
			break;
		case 0xB3:	//ORA E
			ORA(CPU.REG_E);
			break;
		case 0xB4:	//ORA H
			ORA(CPU.REG_H);
			break;
		case 0xB5:	//ORA L
			ORA(CPU.REG_L);
			break;
		case 0xB6:	//ORA M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			ORA(CPU.MEM[addr]);
		}
			break;
		case 0xB7:	//ORA A
			ORA(CPU.REG_A);
			break;
		case 0xB8:	//CMP B
			CMP(CPU.REG_A, CPU.REG_B);
			break;
		case 0xB9:	//CMP C
			CMP(CPU.REG_A, CPU.REG_C);
			break;
		case 0xBA:	//CMP D
			CMP(CPU.REG_A, CPU.REG_D);
			break;
		case 0xBB:	//CMP E
			CMP(CPU.REG_A, CPU.REG_E);
			break;
		case 0xBC:	//CMP H
			CMP(CPU.REG_A, CPU.REG_H);
			break;
		case 0xBD:	//CMP L
			CMP(CPU.REG_A, CPU.REG_L);
			break;
		case 0xBE:	//CMP M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			CMP(CPU.REG_A, CPU.MEM[addr]);
		}
			break;
		case 0xBF:	//CMP A
			CMP(CPU.REG_A, CPU.REG_A);
			break;
		case 0xC0:	//RNZ
			if (!GetZeroFlag())
			{
				RET();
			}
			break;
		case 0xC1:	//POP B
		{
			uint8_t low_order_byte = CPU.MEM[CPU.SP];
			uint8_t high_order_byte = CPU.MEM[CPU.SP + 1];
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_B, &CPU.REG_C, value);
			CPU.SP += 2;// Increment SP by 2
		}
			break;
		case 0xC2:	//JNZ addr
			if (!GetZeroFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xC3:	//JMP addr
			JMP();
			break;
		case 0xC4:	//CNZ addr
			if (!GetZeroFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xC5:	//PUSH B
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_B, CPU.REG_C));
		}
			break;
		case 0xC6:	//ADI d8
			ADD(CPU.REG_A, CPU.MEM[++CPU.PC], 0);
			break;
		case 0xC7:	//RST 0
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0000;
		}
			break;
		case 0xC8:	//RZ
			if (GetZeroFlag())
			{
				RET();
			}
			break;
		case 0xC9:	//RET
			RET();
			break;
		case 0xCA:	//JZ addr
			if (GetZeroFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xCB:	//*JMP addr
			break;
		case 0xCC:	//CZ addr
			if (GetZeroFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xCD:	//CALL addr
			CALL();
			break;
		case 0xCE:	//ACI d8
			ADD(CPU.REG_A, CPU.MEM[++CPU.PC], 1);
			break;
		case 0xCF:	//RST 1
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0008;
		}
			break;
		case 0xD0:	//RNC
			if (!GetCarryFlag())
			{
				RET();
			}
			break;
		case 0xD1:	//POP D
		{
			uint8_t low_order_byte = CPU.MEM[CPU.SP];
			uint8_t high_order_byte = CPU.MEM[CPU.SP + 1];
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_D, &CPU.REG_E, value);
			CPU.SP += 2; // Increment SP by 2
		}
			break;
		case 0xD2:	//JNC addr
			if (!GetCarryFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xD3:	//OUT d8
			IO_OUT();
			break;
		case 0xD4:	//CNC addr
			if (!GetCarryFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xD5:	//PUSH D
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_D, CPU.REG_E));
		}
			break;
		case 0xD6:	//SUI d8
			SUB(CPU.REG_A, CPU.MEM[++CPU.PC], 0);
			break;
		case 0xD7:	//RST 2
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0010;
		}
			break;
		case 0xD8:	//RC
			if (GetCarryFlag())
			{
				RET();
			}
			break;
		case 0xD9:	//*RET
			break;
		case 0xDA:	//JC addr
			if (GetCarryFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xDB:	//IN d8
			IO_IN();
			break;
		case 0xDC:	//CC addr
			if (GetCarryFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xDD:	//*CALL addr
			break;
		case 0xDE:	//SBI d8
			SUB(CPU.REG_A, CPU.MEM[++CPU.PC], 1);
			break;
		case 0xDF:	//RST 3
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0018;
		}
			break;
		case 0xE0:	//RPO
			if (!GetParityFlag())
			{
				RET();
			}
			break;
		case 0xE1:	//POP H
		{
			uint8_t low_order_byte = CPU.MEM[CPU.SP];
			uint8_t high_order_byte = CPU.MEM[CPU.SP + 1];
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_H, &CPU.REG_L, value);
			CPU.SP += 2; // Increment SP by 2
		}
			break;
		case 0xE2:	//JPO addr
			if (!GetParityFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xE3:	//XTHL
		{
			uint8_t SP_value = CPU.MEM[CPU.SP];
			uint8_t SP_1_value = CPU.MEM[CPU.SP + 1];
			uint8_t H = CPU.REG_H;
			uint8_t L = CPU.REG_L;
			CPU.REG_H = SP_1_value;
			CPU.REG_L = SP_value;
			CPU.MEM[CPU.SP] = L;
			CPU.MEM[CPU.SP + 1] = H;
		}
			break;
		case 0xE4:	//CPO addr
			if (!GetParityFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xE5:	//PUSH H
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_H, CPU.REG_L));
		}
			break;
		case 0xE6:	//ANI d8
			ANA(CPU.MEM[++CPU.PC]);
			break;
		case 0xE7:	//RST 4
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0020;
		}
			break;
		case 0xE8:	//RPE
			if (GetParityFlag())
			{
				RET();
			}
			break;
		case 0xE9:	//PCHL
			CPU.PC = CombineRP(CPU.REG_H, CPU.REG_L);
			break;
		case 0xEA:	//JPE addr
			if (GetParityFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xEB:	//XCHG
		{
			uint8_t D = CPU.REG_D;
			uint8_t E = CPU.REG_E;
			uint8_t H = CPU.REG_H;
			uint8_t L = CPU.REG_L;
			CPU.REG_H = D;
			CPU.REG_L = E;
			CPU.REG_D = H;
			CPU.REG_E = L;
		}
			break;
		case 0xEC:	//CPE addr
			if (GetParityFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xED:	//*CALL addr
			break;
		case 0xEE:	//XRI d8
			XRA(CPU.MEM[++CPU.PC]);
			break;
		case 0xEF:	//RST 5
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0028;
		}
			break;
		case 0xF0:	//RP
			if (!GetSignFlag())
			{
				RET();
			}
			break;
		case 0xF1:	//POP PSW
		{
			uint8_t low_order_byte = CPU.MEM[CPU.SP];
			uint8_t high_order_byte = CPU.MEM[CPU.SP + 1];
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_A, &CPU.FLAGS, value);
			CPU.SP += 2;// Increment SP by 2
		}
			break;
		case 0xF2:	//JP addr
			if (!GetSignFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xF3:	//DI
			CPU.INT = 0;
			break;
		case 0xF4:	//CP addr
			if (!GetSignFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xF5:	//PUSH PSW
		{
			uint16_t A_addr = --CPU.SP;
			uint16_t FLAGS_addr = --CPU.SP;
			StoreRP(&CPU.MEM[A_addr], &CPU.MEM[FLAGS_addr], CombineRP(CPU.REG_A, CPU.FLAGS));
		}
			break;
		case 0xF6:	//ORI d8
			ORA(CPU.MEM[++CPU.PC]);
			break;
		case 0xF7:	//RST 6
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0030;
		}
			break;
		case 0xF8:	//RM
			if (GetSignFlag())
			{
				RET();
			}
			break;
		case 0xF9:	//SPHL
			CPU.SP = CombineRP(CPU.REG_H, CPU.REG_L);
			break;
		case 0xFA:	//JM addr
			if (GetSignFlag())
			{
				JMP();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xFB:	//EI
			CPU.INT = 1;
			break;
		case 0xFC:	//CM addr
			if (GetSignFlag())
			{
				CALL();
			}
			else
			{
				CPU.PC += 2;
			}
			break;
		case 0xFD:	//*CALL addr
			break;
		case 0xFE:	//CPI d8
			CMP(CPU.REG_A, CPU.MEM[++CPU.PC]);
			break;
		case 0xFF:	//RST 7
		{
			uint16_t high_order_addr = --CPU.SP;
			uint16_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0038;
		}
			break;
	}

	CPU.PC++; // Increment program counter
	//PrintCPUState();
}

void LoadProgram()
{
	int file_read = 0;
	uint8_t* object_file_data = NULL;
	uint16_t file_size = 0;
	char file_name[256];

	while (!file_read)
	{
		printf("Give program file name: ");
		scanf_s("%255s", file_name, 256);
		fseek(stdin, 0, SEEK_END);
		file_read = LoadFile(file_name, &object_file_data, &file_size);
	}


	CPU.PC = 0x0000;
	memcpy(&CPU.MEM[CPU.PC], object_file_data, file_size);

	printf("%s loaded at 0x%X\n\n", file_name, CPU.PC);

	free(object_file_data);
	object_file_data = NULL;
}

void StepThroughProgram(uint64_t *instructions_executed)
{
	int32_t intstructions_to_run = 0;
	int option = 0;
	char code_line[CODE_LINE_LENGTH];
	char print_line[DISASSEMBLY_LINE_PREFIX_LENGTH + CODE_LINE_LENGTH];

	while (!test_finished)
	{
		if (intstructions_to_run <= 0)
		{
			while (1)
			{
				printf("\n");
				GetDisassemblyLine(&CPU.MEM, CPU.PC, print_line);
				
				printf("1 Execute the next instruction [ %s]\n2 Execute specified amount of instructions\n3 Print CPU state\n", print_line);
				scanf_s("%1d", &option);
				fseek(stdin, 0, SEEK_END);

				if (option == 1)
				{
					printf("\n");
					break;
				}
				else if (option == 3)
				{
					break;
				}
				else if (option == 2)
				{
					printf("Give amount of instructions to execute\n");
					scanf_s("%d", &intstructions_to_run);
					fseek(stdin, 0, SEEK_END);
					printf("\n");
					break;
				}
			}
		}

		// Print disassembly of code to be executed
		if (option == 1 || option == 2)
		{
			printf("%llu: ", *instructions_executed + 1);
			GetDisassemblyLine(&CPU.MEM, CPU.PC, print_line);
			printf("%s\n", print_line);

			Execute();

			*instructions_executed += 1;
			intstructions_to_run -= 1;
		}
		// Print CPU state
		else if (option == 3)
		{
			PrintCPUState();
		}
	}
}

void Run()
{

	LoadProgram();

	
	int run_option = 0;

	while (1)
	{
		printf("1. Run program\n2. Step through program\n");
		scanf_s("%1d", &run_option);
		fseek(stdin, 0, SEEK_END);

		if (run_option == 1 || run_option == 2)
		{
			break;
		}
	}

	uint64_t cycle_start_time = ns(), cycle_end_time = ns(), cycle_time = 0;

	const uint64_t clock_frq = 2'000'000;
	uint64_t clock_in_ns = 1e9 / clock_frq;

	uint64_t instructions_executed = 0;

	test_finished = false;

	// Invaders stuck in infinite loop after instruction 42044 (waiting for IO)

	// Run a program
	if (run_option == 0)
	{
		while (!test_finished)
		{
			cycle_time = cycle_end_time - cycle_start_time;
			if (cycle_time >= clock_in_ns)
			{
				//printf("cycle time: %lluns\n----------------------\n", cycle_time);
				cycle_start_time = ns();

				Execute();
				instructions_executed += 1;
			}
			cycle_end_time = ns();
		}
		printf("Test finished\nInstructions executed: %llu\n", instructions_executed);
	}
	// Step through a program
	else if(run_option == 2)
	{
		StepThroughProgram(&instructions_executed);
		printf("Test finished\nInstructions executed: %llu\n", instructions_executed);
	}
}
