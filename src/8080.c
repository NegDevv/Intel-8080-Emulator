#include "8080.h"
#include "disassembler.h"
#include "timer.h"


const byte instruction_cycles_table[256] = {
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

const byte instruction_cycles_table_secondary[256] = {
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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void PrintCPUState()
{
	printf("\nPC: 0x%X", CPU.PC);
	printf("\nSP: 0x%X", CPU.SP);

	printf("\nFLAGS: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(CPU.FLAGS));

	/*printf("\nA: 0x%02X (%u)", CPU.REG_A, CPU.REG_A);
	printf("\nB: 0x%02X (%u)", CPU.REG_B, CPU.REG_B);
	printf("\nC: 0x%02X (%u)", CPU.REG_C, CPU.REG_C);
	printf("\nD: 0x%02X (%u)", CPU.REG_D, CPU.REG_D);
	printf("\nE: 0x%02X (%u)", CPU.REG_E, CPU.REG_E);
	printf("\nH: 0x%02X (%u)", CPU.REG_H, CPU.REG_H);
	printf("\nL: 0x%02X (%u)", CPU.REG_L, CPU.REG_L);*/

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
}

// Changes n bit to value indicated by parameter bit. n = 0 is the first bit
uint8_t ChangeNBit(uint8_t num, uint8_t n, uint8_t bit)
{
	return num & ~(1 << n) | (bit << n);
}

void ZeroFlag(byte result)
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 6, (result == 0));
}

void ParityFlag(byte result)
{
	uint8_t set_bits = 0;
	// Count number of set (1) bits
	for (uint8_t i = 0; i < 8; i++)
	{
		set_bits += ((result >> i) & 1);
	}
	// Sets parity flag (1) if number of set (1) bits is even, otherwise parity flag is reset (0)
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 2, ((set_bits & 1) == 0));
}

uint8_t GetCarryFlag()
{
	return (CPU.FLAGS & 0b00000001) >> 0;
}

uint8_t GetParityFlag()
{
	return (CPU.FLAGS & 0b00000100) >> 2;
}

uint8_t GetAuxCarryFlag()
{
	return (CPU.FLAGS & 0b00010000) >> 4;
}

uint8_t GetZeroFlag()
{
	return (CPU.FLAGS & 0b01000000) >> 6;
}

uint8_t GetSignFlag()
{
	return (CPU.FLAGS & 0b10000000) >> 7;
}


void ClearAuxCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, 0);
}

void ClearCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, 0);
}

void SetAuxCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, 1);
}

void SetCarryFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, 1);
}

void SignFlag(byte result)
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 7, (result & 0b10000000));
}

void SetZeroFlag()
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 6, 1);
}

// Aux Carry flag set/unset for addition/subtraction/comparison
void AuxCarryFlag(byte value_1, byte value_2, uint8_t operation)
{
	// Addition
	if (operation == 0)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, ((value_1 & 0x0F) + (value_2 & 0x0F) > 0x0F));
	}
	// Subtraction
	else if (operation == 1)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, ((value_1 & 0x0F) < (value_2 & 0x0F)));
	}
	// Compare
	else if(operation == 2)
	{
		int16_t result = value_1 - value_2;
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, ~(value_1 ^ result ^ value_2) & 0x10);
	}
}

void AuxCarryFlagAND(byte value_1, byte value_2)
{
	CPU.FLAGS = ChangeNBit(CPU.FLAGS, 4, ((value_1 | value_2) & 0x08) != 0);
}

// Carry flag set/unset for single precision (8-bit) addition/subtraction/comparison
void CarryFlagSingle(uint16_t value_1, uint16_t value_2, uint8_t operation)
{
	// Addition
	if (operation == 0)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (((uint16_t)value_1 + value_2) & 0x0100) != 0);
	}
	// Subtraction
	else if(operation == 1)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (((uint16_t)value_1 - value_2) & 0x0100) != 0);
		
	}
	// Compare
	else if (operation == 2)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, value_2 > 0xFF);
	}
}

// Carry flag set/unset for double precision (16-bit) addition/subtraction
void CarryFlagDouble(uint32_t value_1, uint32_t value_2, uint8_t subtraction)
{
	if (subtraction == 1)
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (((uint32_t)value_1 - value_2) & 0x10000) != 0);
	}
	else
	{
		CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (((uint32_t)value_1 + value_2) & 0x10000) != 0);
	}
}

// Combine register pair into one 16-bit value
uint16_t CombineRP(uint8_t high_order_byte, uint8_t low_order_byte)
{
	return (uint16_t)(high_order_byte << 8) | low_order_byte;
}

// Store 16-bit value between two 8-bit registers
uint16_t StoreRP(uint8_t* high_order_register, uint8_t* low_order_register, uint16_t number)
{
	*high_order_register = number >> 8;
	*low_order_register = number & 0x00FF;
}

void CMP(byte value_1, byte value_2)
{
	CarryFlagSingle(CPU.REG_A, CPU.REG_H, 0);
	AuxCarryFlag(CPU.REG_A, CPU.REG_H, 0);
	CPU.REG_A += CPU.REG_H;
	ZeroFlag(CPU.REG_A);
	SignFlag(CPU.REG_A);
	ParityFlag(CPU.REG_A);
}

void RET()
{
	uint8_t low_order_byte = CPU.MEM[CPU.SP];
	uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
	uint16_t addr = CombineRP(high_order_byte, low_order_byte);
	CPU.PC = addr;
	CPU.SP += 1; // Increment SP by 1
}

void JMP()
{
	// Since program counter gets incremented at the end of the Execute function decrement target PC by 1 for all branch instructons
	// (less typing since most instructions dont manipulate PC directly)
	CPU.PC = ((uint16_t)(CPU.MEM[++CPU.PC]) | CPU.MEM[++CPU.PC] << 8) - 1;
}

void CALL()
{
	uint16_t next_instruction_addr = CPU.PC + 3;
	uint8_t high_order_addr = --CPU.SP;
	uint8_t low_order_addr = --CPU.SP;
	StoreRP(CPU.MEM[high_order_addr], CPU.MEM[low_order_addr], next_instruction_addr);
	// Since program counter gets incremented at the end of the Execute function decrement target PC by 1 for all branch instructons
	// (less typing since most instructions dont manipulate PC directly)
	CPU.PC = ((uint16_t)(CPU.MEM[++CPU.PC]) | CPU.MEM[++CPU.PC] << 8) - 1;
}


void Execute()
{
	//printf("Execute\n");
	
	// Get next instruction
	byte op_code = CPU.MEM[CPU.PC];
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
			AuxCarryFlag(CPU.REG_B, 1, 0);
			CPU.REG_B += 1;
			ZeroFlag(CPU.REG_B);
			SignFlag(CPU.REG_B);
			ParityFlag(CPU.REG_B);
			break;
		case 0x05:	//DCR B
			AuxCarryFlag(CPU.REG_B, 1, 1);
			CPU.REG_B -= 1;
			ZeroFlag(CPU.REG_B);
			SignFlag(CPU.REG_B);
			ParityFlag(CPU.REG_B);
			break;
		case 0x06:	//MVI B,d8
			CPU.REG_B = CPU.MEM[++CPU.PC];
			break;
		case 0x07:	//RLC
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, ((CPU.REG_A & 0b1000'0000) >> 7));
			CPU.REG_A = (CPU.REG_A << 1) | GetCarryFlag();
			break;
		case 0x08:	//*NOP
			break;
		case 0x09:	//DAD B
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t BC = CombineRP(CPU.REG_B, CPU.REG_C);
			uint32_t result = (uint32_t)HL + BC;
			StoreRP(&CPU.REG_H, &CPU.REG_L, result & 0xFFFF);
			CarryFlagDouble(HL, BC, 0);
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
			AuxCarryFlag(CPU.REG_C, 1, 0);
			CPU.REG_C += 1;
			ZeroFlag(CPU.REG_C);
			SignFlag(CPU.REG_C);
			ParityFlag(CPU.REG_C);
			break;
		case 0x0D:	//DCR C
			AuxCarryFlag(CPU.REG_C, 1, 0);
			CPU.REG_C -= 1;
			ZeroFlag(CPU.REG_C);
			SignFlag(CPU.REG_C);
			ParityFlag(CPU.REG_C);
			break;
		case 0x0E:	//MVI C,d8
			CPU.REG_C = CPU.MEM[++CPU.PC];
			break;
		case 0x0F:	//RRC
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (CPU.REG_A & 0x1));
			CPU.REG_A = (CPU.REG_A >> 1) | (GetCarryFlag() << 7);
			break;
		case 0x10:	//*NOP
			break;
		case 0x11:	//LXI D,d16
			CPU.REG_D = CPU.MEM[++CPU.PC];
			CPU.REG_C = CPU.MEM[++CPU.PC];
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
			AuxCarryFlag(CPU.REG_D, 1, 0);
			CPU.REG_D += 1;
			ZeroFlag(CPU.REG_D);
			SignFlag(CPU.REG_D);
			ParityFlag(CPU.REG_D);
			break;
		case 0x15:	//DCR D
			AuxCarryFlag(CPU.REG_D, 1, 1);
			CPU.REG_D -= 1;
			ZeroFlag(CPU.REG_D);
			SignFlag(CPU.REG_D);
			ParityFlag(CPU.REG_D);
			break;
		case 0x16:	//MVI D,d8
			CPU.REG_D = CPU.MEM[++CPU.PC];
			break;
		case 0x17:	//RAL
		{
			uint8_t carry = GetCarryFlag(); // Get old carry flag
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, ((CPU.REG_A & 0b1000'0000) >> 7)); // Update carry flag
			CPU.REG_A = (CPU.REG_A << 1) | carry; // Shift left and set low order bit to old carry flag
		}
			break;
		case 0x18:	//*NOP
			break;
		case 0x19:	//DAD D
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t DE = CombineRP(CPU.REG_D, CPU.REG_E);
			uint32_t result = (uint32_t)HL + DE;
			StoreRP(&CPU.REG_H, &CPU.REG_L, result & 0xFFFF);
			CarryFlagDouble(HL, DE, 0);
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
			AuxCarryFlag(CPU.REG_E, 1, 0);
			CPU.REG_E += 1;
			ZeroFlag(CPU.REG_E);
			SignFlag(CPU.REG_E);
			ParityFlag(CPU.REG_E);
			break;
		case 0x1D:	//DCR E
			AuxCarryFlag(CPU.REG_E, 1, 1);
			CPU.REG_E -= 1;
			ZeroFlag(CPU.REG_E);
			SignFlag(CPU.REG_E);
			ParityFlag(CPU.REG_E);
			break;
		case 0x1E:	//MVI E,d8
			CPU.REG_E = CPU.MEM[++CPU.PC];
			break;
		case 0x1F:	//RAR
		{
			uint8_t carry = GetCarryFlag(); // Get old carry flag
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, (CPU.REG_A & 0x1)); // Update carry flag
			CPU.REG_A = (CPU.REG_A >> 1) | (carry << 7); // Shift right and set high order bit to old carry flag
		}
			break;
		case 0x20:	//*NOP
			break;
		case 0x21:	//LXI H,d16
			CPU.REG_H = CPU.MEM[++CPU.PC];
			CPU.REG_L = CPU.MEM[++CPU.PC];
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
			AuxCarryFlag(CPU.REG_H, 1, 0);
			CPU.REG_H += 1;
			ZeroFlag(CPU.REG_H);
			SignFlag(CPU.REG_H);
			ParityFlag(CPU.REG_H);
			break;
		case 0x25:	//DCR H
			AuxCarryFlag(CPU.REG_H, 1, 1);
			CPU.REG_H -= 1;
			ZeroFlag(CPU.REG_H);
			SignFlag(CPU.REG_H);
			ParityFlag(CPU.REG_H);
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

			//CarryFlagSingle(CPU.REG_A, correction, 0);
			AuxCarryFlag(CPU.REG_A, correction, 0);
			CPU.REG_A += correction;
			CPU.FLAGS = ChangeNBit(CPU.FLAGS, 0, carry); // Set carry bit
		}
			
			break;
		case 0x28:	//*NOP
			break;
		case 0x29:	//DAD H
		{
			uint16_t HL = CombineRP(CPU.REG_H, CPU.REG_L);
			uint16_t HL_2 = CombineRP(CPU.REG_H, CPU.REG_L);
			uint32_t result = (uint32_t)HL + HL_2;
			StoreRP(&CPU.REG_H, &CPU.REG_L, result & 0xFFFF);
			CarryFlagDouble(HL, HL_2, 0);
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
			AuxCarryFlag(CPU.REG_L, 1, 0);
			CPU.REG_L += 1;
			ZeroFlag(CPU.REG_L);
			SignFlag(CPU.REG_L);
			ParityFlag(CPU.REG_L);
			break;
		case 0x2D:	//DCR L
			AuxCarryFlag(CPU.REG_L, 1, 1);
			CPU.REG_L -= 1;
			ZeroFlag(CPU.REG_L);
			SignFlag(CPU.REG_L);
			ParityFlag(CPU.REG_L);
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
			byte temp = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			AuxCarryFlag(temp, 1, 0);
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] += 1;
			ZeroFlag(temp);
			SignFlag(temp);
			ParityFlag(temp);
		}
			break;
		case 0x35:	//DCR M
		{
			byte temp = CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			AuxCarryFlag(temp, 1, 1);
			CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)] -= 1;
			ZeroFlag(temp);
			SignFlag(temp);
			ParityFlag(temp);
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
			uint32_t result = (uint32_t)HL + CPU.SP;
			StoreRP(&CPU.REG_H, &CPU.REG_L, result & 0xFFFF);
			CarryFlagDouble(HL, CPU.SP, 0);
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
			AuxCarryFlag(CPU.REG_A, 1, 0);
			CPU.REG_A += 1;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x3D:	//DCR A
			AuxCarryFlag(CPU.REG_A, 1, 1);
			CPU.REG_A -= 1;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x3E:	//MVI A,d8
			CPU.REG_A = CPU.MEM[++CPU.PC];
			break;
		case 0x3F:	//CMC
			ChangeNBit(CPU.FLAGS, 0, !GetCarryFlag());
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
			CarryFlagSingle(CPU.REG_A, CPU.REG_B, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_B, 0);
			CPU.REG_A += CPU.REG_B;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x81:	//ADD C
			CarryFlagSingle(CPU.REG_A, CPU.REG_C, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_C, 0);
			CPU.REG_A += CPU.REG_C;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x82:	//ADD D
			CarryFlagSingle(CPU.REG_A, CPU.REG_D, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_D, 0);
			CPU.REG_A += CPU.REG_D;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x83:	//ADD E
			CarryFlagSingle(CPU.REG_A, CPU.REG_E, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_E, 0);
			CPU.REG_A += CPU.REG_E;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x84:	//ADD H
			CarryFlagSingle(CPU.REG_A, CPU.REG_H, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_H, 0);
			CPU.REG_A += CPU.REG_H;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x85:	//ADD L
			CarryFlagSingle(CPU.REG_A, CPU.REG_L, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_L, 0);
			CPU.REG_A += CPU.REG_L;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x86:	//ADD M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			CarryFlagSingle(CPU.REG_A, CPU.MEM[addr], 0);
			AuxCarryFlag(CPU.REG_A, CPU.MEM[addr], 0);
			CPU.REG_A += CPU.MEM[addr];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			
			break;
		case 0x87:	//ADD A
			CarryFlagSingle(CPU.REG_A, CPU.REG_A, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_A, 0);
			CPU.REG_A += CPU.REG_A;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x88:	//ADC B
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_B + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_B + carry_bit, 0);
			CPU.REG_A += CPU.REG_B + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x89:	//ADC C
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_C + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_C + carry_bit, 0);
			CPU.REG_A += CPU.REG_C + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8A:	//ADC D
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_D + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_D + carry_bit, 0);
			CPU.REG_A += CPU.REG_D + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8B:	//ADC E
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_E + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_E + carry_bit, 0);
			CPU.REG_A += CPU.REG_E + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8C:	//ADC H
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_H + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_H + carry_bit, 0);
			CPU.REG_A += CPU.REG_H + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8D:	//ADC L
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_L + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_L + carry_bit, 0);
			CPU.REG_A += CPU.REG_L + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8E:	//ADC M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.MEM[addr] + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.MEM[addr] + carry_bit, 0);
			CPU.REG_A += CPU.MEM[addr] + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x8F:	//ADC A
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_A + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, CPU.REG_A + carry_bit, 0);
			CPU.REG_A += CPU.REG_A + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x90:	//SUB B
			CarryFlagSingle(CPU.REG_A, CPU.REG_B, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_B, 1);
			CPU.REG_A -= CPU.REG_B;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x91:	//SUB C
			CarryFlagSingle(CPU.REG_A, CPU.REG_C, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_C, 1);
			CPU.REG_A -= CPU.REG_C;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x92:	//SUB D
			CarryFlagSingle(CPU.REG_A, CPU.REG_D, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_D, 1);
			CPU.REG_A -= CPU.REG_D;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x93:	//SUB E
			CarryFlagSingle(CPU.REG_A, CPU.REG_E, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_E, 1);
			CPU.REG_A -= CPU.REG_E;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x94:	//SUB H
			CarryFlagSingle(CPU.REG_A, CPU.REG_H, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_H, 1);
			CPU.REG_A -= CPU.REG_H;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x95:	//SUB L
			CarryFlagSingle(CPU.REG_A, CPU.REG_L, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_L, 1);
			CPU.REG_A -= CPU.REG_L;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x96:	//SUB M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			CarryFlagSingle(CPU.REG_A, CPU.MEM[addr], 1);
			AuxCarryFlag(CPU.REG_A, CPU.MEM[addr], 1);
			CPU.REG_A -= CPU.MEM[addr];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x97:	//SUB A
			CarryFlagSingle(CPU.REG_A, CPU.REG_A, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_A, 1);
			CPU.REG_A -= CPU.REG_A;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0x98:	//SBB B
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_B + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_B + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_B + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x99:	//SBB C
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_C + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_C + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_C + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9A:	//SBB D
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_D + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_D + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_D + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9B:	//SBB E
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_E + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_E + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_E + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9C:	//SBB H
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_H + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_H + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_H + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9D:	//SBB L
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_L + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_L + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_L + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9E:	//SBB M
		{
			uint8_t carry_bit = GetCarryFlag();
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			CarryFlagSingle(CPU.REG_A, CPU.MEM[addr] + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.MEM[addr] + carry_bit, 1);
			CPU.REG_A -= (CPU.MEM[addr] + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0x9F:	//SBB A
		{
			uint8_t carry_bit = GetCarryFlag();
			CarryFlagSingle(CPU.REG_A, CPU.REG_A + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, CPU.REG_A + carry_bit, 1);
			CPU.REG_A -= (CPU.REG_A + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xA0:	//ANA B
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_B);
			CPU.REG_A &= CPU.REG_B;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA1:	//ANA C
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_C);
			CPU.REG_A &= CPU.REG_C;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA2:	//ANA D
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_D);
			CPU.REG_A &= CPU.REG_D;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA3:	//ANA E
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_E);
			CPU.REG_A &= CPU.REG_E;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA4:	//ANA H
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_H);
			CPU.REG_A &= CPU.REG_H;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA5:	//ANA L
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_L);
			CPU.REG_A &= CPU.REG_L;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA6:	//ANA M
			AuxCarryFlagAND(CPU.REG_A, CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)]);
			CPU.REG_A &= CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA7:	//ANA A
			AuxCarryFlagAND(CPU.REG_A, CPU.REG_A);
			CPU.REG_A &= CPU.REG_A;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
			break;
		case 0xA8:	//XRA B
			CPU.REG_A ^= CPU.REG_B;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xA9:	//XRA C
			CPU.REG_A ^= CPU.REG_C;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAA:	//XRA D
			CPU.REG_A ^= CPU.REG_D;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAB:	//XRA E
			CPU.REG_A ^= CPU.REG_E;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAC:	//XRA H
			CPU.REG_A ^= CPU.REG_H;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAD:	//XRA L
			CPU.REG_A ^= CPU.REG_L;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAE:	//XRA M
			CPU.REG_A ^= CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xAF:	//XRA A
			CPU.REG_A ^= CPU.REG_A;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB0:	//ORA B
			CPU.REG_A |= CPU.REG_B;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB1:	//ORA C
			CPU.REG_A |= CPU.REG_C;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB2:	//ORA D
			CPU.REG_A |= CPU.REG_D;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB3:	//ORA E
			CPU.REG_A |= CPU.REG_E;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB4:	//ORA H
			CPU.REG_A |= CPU.REG_H;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB5:	//ORA L
			CPU.REG_A |= CPU.REG_L;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB6:	//ORA M
			CPU.REG_A |= CPU.MEM[CombineRP(CPU.REG_H, CPU.REG_L)];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB7:	//ORA A
			CPU.REG_A |= CPU.REG_A;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xB8:	//CMP B
			CarryFlagSingle(CPU.REG_A, CPU.REG_B, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_B, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_B);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xB9:	//CMP C
			CarryFlagSingle(CPU.REG_A, CPU.REG_C, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_C, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_C);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xBA:	//CMP D
			CarryFlagSingle(CPU.REG_A, CPU.REG_D, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_D, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_D);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xBB:	//CMP E
			CarryFlagSingle(CPU.REG_A, CPU.REG_E, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_E, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_E);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xBC:	//CMP H
			CarryFlagSingle(CPU.REG_A, CPU.REG_H, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_H, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_H);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xBD:	//CMP L
			CarryFlagSingle(CPU.REG_A, CPU.REG_L, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_L, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_L);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xBE:	//CMP M
		{
			uint16_t addr = CombineRP(CPU.REG_H, CPU.REG_L);
			CarryFlagSingle(CPU.REG_A, CPU.MEM[addr], 2);
			AuxCarryFlag(CPU.REG_A, CPU.MEM[addr], 2);
			ZeroFlag(CPU.REG_A != CPU.MEM[addr]);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xBF:	//CMP A
			CarryFlagSingle(CPU.REG_A, CPU.REG_A, 2);
			AuxCarryFlag(CPU.REG_A, CPU.REG_A, 2);
			ZeroFlag(CPU.REG_A != CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			break;
		case 0xC0:	//RNZ
			if (!GetCarryFlag())
			{
				RET();
			}
			break;
		case 0xC1:	//POP B
		{
			uint8_t low_order_byte = CPU.MEM[CPU.SP];
			uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_B, &CPU.REG_C, value);
			CPU.SP += 1;// Increment SP by 1
		}
			break;
		case 0xC2:	//JNZ addr
			if (!GetZeroFlag())
			{
				JMP();
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
			break;
		case 0xC5:	//PUSH B
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_B, CPU.REG_C));
		}
			break;
		case 0xC6:	//ADI d8
		{
			uint8_t d8 = CPU.MEM[++CPU.PC];
			CarryFlagSingle(CPU.REG_A, d8, 0);
			AuxCarryFlag(CPU.REG_A, d8, 0);
			CPU.REG_A += d8;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xC7:	//RST 0
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			break;
		case 0xCB:	//*JMP addr
			break;
		case 0xCC:	//CZ addr
			if (GetZeroFlag())
			{
				CALL();
			}
			break;
		case 0xCD:	//CALL addr
			CALL();
			break;
		case 0xCE:	//ACI d8
		{
			uint8_t carry_bit = GetCarryFlag();
			uint8_t d8 = CPU.MEM[++CPU.PC];
			CarryFlagSingle(CPU.REG_A, d8 + carry_bit, 0);
			AuxCarryFlag(CPU.REG_A, d8 + carry_bit, 0);
			CPU.REG_A += d8 + carry_bit;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xCF:	//RST 1
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_D, &CPU.REG_E, value);
			CPU.SP += 1;// Increment SP by 1
		}
			break;
		case 0xD2:	//JNC addr
			if (!GetCarryFlag())
			{
				JMP();
			}
			break;
		case 0xD3:	//OUT d8
			CPU.IO_OUT[CPU.MEM[++CPU.PC]] = CPU.REG_A;
			break;
		case 0xD4:	//CNC addr
			if (!GetCarryFlag())
			{
				CALL();
			}
			break;
		case 0xD5:	//PUSH D
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_D, CPU.REG_E));
		}
			break;
		case 0xD6:	//SUI d8
		{
			uint8_t d8 = CPU.MEM[++CPU.PC];
			CarryFlagSingle(CPU.REG_A, d8, 1);
			AuxCarryFlag(CPU.REG_A, d8, 1);
			CPU.REG_A -= d8;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xD7:	//RST 2
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			break;
		case 0xDB:	//IN d8
			CPU.REG_A = CPU.IO_IN[CPU.MEM[++CPU.PC]];
			break;
		case 0xDC:	//CC addr
			if (GetCarryFlag())
			{
				CALL();
			}
			break;
		case 0xDD:	//*CALL addr
			break;
		case 0xDE:	//SBI d8
		{
			uint8_t carry_bit = GetCarryFlag();
			uint8_t d8 = CPU.MEM[++CPU.PC];
			CarryFlagSingle(CPU.REG_A, d8 + carry_bit, 1);
			AuxCarryFlag(CPU.REG_A, d8 + carry_bit, 1);
			CPU.REG_A -= (d8 + carry_bit);
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xDF:	//RST 3
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_H, &CPU.REG_L, value);
			CPU.SP += 1;// Increment SP by 1
		}
			break;
		case 0xE2:	//JPO addr
			if (!GetParityFlag())
			{
				JMP();
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
			if (GetParityFlag())
			{
				CALL();
			}
			break;
		case 0xE5:	//PUSH H
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], CombineRP(CPU.REG_H, CPU.REG_L));
		}
			break;
		case 0xE6:	//ANI d8
		{
			uint8_t d8 = CPU.MEM[++CPU.PC];
			AuxCarryFlagAND(CPU.REG_A, d8);
			CPU.REG_A &= d8;
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearCarryFlag();
		}
			
			break;
		case 0xE7:	//RST 4
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			break;
		case 0xED:	//*CALL addr
			break;
		case 0xEE:	//XRI d8
			CPU.REG_A ^= CPU.MEM[++CPU.PC];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xEF:	//RST 5
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			uint8_t high_order_byte = CPU.MEM[++CPU.SP]; // Increment SP by 1
			uint16_t value = CombineRP(high_order_byte, low_order_byte);
			StoreRP(&CPU.REG_A, &CPU.FLAGS, value);
			CPU.SP += 1;// Increment SP by 1
		}
			break;
		case 0xF2:	//JP addr
			if (!GetSignFlag())
			{
				JMP();
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
			break;
		case 0xF5:	//PUSH PSW
		{
			uint8_t A_addr = --CPU.SP;
			uint8_t FLAGS_addr = --CPU.SP;
			StoreRP(&CPU.MEM[A_addr], &CPU.MEM[FLAGS_addr], CombineRP(CPU.REG_A, CPU.FLAGS));
		}
			break;
		case 0xF6:	//ORI d8
			CPU.REG_A |= CPU.MEM[++CPU.PC];
			ZeroFlag(CPU.REG_A);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
			ClearAuxCarryFlag();
			ClearCarryFlag();
			break;
		case 0xF7:	//RST 6
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
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
			break;
		case 0xFB:	//EI
			CPU.INT = 1;
			break;
		case 0xFC:	//CM addr
			if (GetSignFlag())
			{
				CALL();
			}
			break;
		case 0xFD:	//*CALL addr
			break;
		case 0xFE:	//CPI d8
		{
			uint8_t d8 = CPU.MEM[++CPU.PC];
			CarryFlagSingle(CPU.REG_A, d8, 2);
			AuxCarryFlag(CPU.REG_A, d8, 2);
			ZeroFlag(CPU.REG_A != d8);
			SignFlag(CPU.REG_A);
			ParityFlag(CPU.REG_A);
		}
			break;
		case 0xFF:	//RST 7
		{
			uint8_t high_order_addr = --CPU.SP;
			uint8_t low_order_addr = --CPU.SP;
			uint16_t next_instruction_addr = CPU.PC + 1;
			StoreRP(&CPU.MEM[high_order_addr], &CPU.MEM[low_order_addr], next_instruction_addr);
			CPU.PC = 0x0038;
		}
			break;
	}

	CPU.PC++; // Increment program counter
	PrintCPUState();
}

void LoadProgram()
{
	int file_read = 0;
	byte* object_file_data = NULL;
	uint16_t file_size = 0;
	char file_name[256];

	while (!file_read)
	{
		printf("Give program file name: ");
		scanf_s("%255s", file_name, 256);
		file_read = LoadFile(file_name, &object_file_data, &file_size);
	}

	CPU.PC = 0x100;
	memcpy(&CPU.MEM[CPU.PC], object_file_data, file_size);

	printf("%s loaded at 0x%X\n\n", file_name, CPU.PC);

	free(object_file_data);
	object_file_data = NULL;
}

void Run()
{

	LoadProgram();

	uint64_t cycle_start_time = ns(), cycle_end_time = ns(), cycle_time = 0;


	const uint64_t clock_frq = 2'000'000;
	uint64_t clock_in_ns = 1e9 / clock_frq;


	while (1)
	{
		cycle_time = cycle_end_time - cycle_start_time;
		if (cycle_time >= clock_in_ns)
		{
			printf("cycle time: %lluns\n", cycle_time);
			cycle_start_time = ns();

			Execute();
		}
		cycle_end_time = ns();
	}

}
