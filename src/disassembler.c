#include "disassembler.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

const char* instruction_table[256] = { "NOP", "LXI B,d16", "STAX B", "INX B", "INR B", "DCR B", "MVI B,d8", "RLC", "*NOP", "DAD B", "LDAX B", "DCX B", "INR C", "DCR C", "MVI C,d8", "RRC",
"*NOP", "LXI D,d16", "STAX D", "INX D", "INR D", "DCR D", "MVI D,d8", "RAL", "*NOP", "DAD D", "LDAX D", "DCX D", "INR E", "DCR E", "MVI E,d8", "RAR",
"*NOP", "LXI H,d16", "SHLD addr", "INX H", "INR H", "DCR H", "MVI H,d8", "DAA", "*NOP", "DAD H", "LHLD addr", "DCX H", "INR L", "DCR L", "MVI L,d8", "CMA",
"*NOP", "LXI SP,d16", "STA addr", "INX SP", "INR M", "DCR M", "MVI M,d8", "STC", "*NOP", "DAD SP", "LDA addr", "DCX SP", "INR A", "DCR A", "MVI A,d8", "CMC",
"MOV B,B", "MOV B,C", "MOV B,D", "MOV B,E", "MOV B,H", "MOV B,L", "MOV B,M", "MOV B,A", "MOV C,B", "MOV C,C", "MOV C,D", "MOV C,E", "MOV C,H", "MOV C,L", "MOV C,M", "MOV C,A",
"MOV D,B", "MOV D,C", "MOV D,D", "MOV D,E", "MOV D,H", "MOV D,L", "MOV D,M", "MOV D,A", "MOV E,B", "MOV E,C", "MOV E,D", "MOV E,E", "MOV E,H", "MOV E,L", "MOV E,M", "MOV E,A",
"MOV H,B", "MOV H,C", "MOV H,D", "MOV H,E", "MOV H,H", "MOV H,L", "MOV H,M", "MOV H,A", "MOV L,B", "MOV L,C", "MOV L,D", "MOV L,E", "MOV L,H", "MOV L,L", "MOV L,M", "MOV L,A",
"MOV M,B", "MOV M,C", "MOV M,D", "MOV M,E", "MOV M,H", "MOV M,L", "HLT", "MOV M,A", "MOV A,B", "MOV A,C", "MOV A,D", "MOV A,E", "MOV A,H", "MOV A,L", "MOV A,M", "MOV A,A",
"ADD B", "ADD C", "ADD D", "ADD E", "ADD H", "ADD L", "ADD M", "ADD A", "ADC B", "ADC C", "ADC D", "ADC E", "ADC H", "ADC L", "ADC M", "ADC A",
"SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB M", "SUB A", "SBB B", "SBB C", "SBB D", "SBB E", "SBB H", "SBB L", "SBB M", "SBB A",
"ANA B", "ANA C", "ANA D", "ANA E", "ANA H", "ANA L", "ANA M", "ANA A","XRA B", "XRA C", "XRA D", "XRA E", "XRA H", "XRA L", "XRA M", "XRA A",
"ORA B", "ORA C", "ORA D", "ORA E", "ORA H", "ORA L", "ORA M", "ORA A","CMP B", "CMP C", "CMP D", "CMP E", "CMP H", "CMP L", "CMP M", "CMP A",
"RNZ", "POP B", "JNZ addr", "JMP addr", "CNZ addr", "PUSH B", "ADI d8", "RST 0", "RZ", "RET", "JZ addr", "*JMP addr", "CZ addr", "CALL addr", "ACI d8", "RST 1",
"RNC", "POP D", "JNC addr", "OUT d8", "CNC addr", "PUSH D", "SUI d8", "RST 2","RC", "*RET", "JC addr", "IN d8", "CC addr", "*CALL addr", "SBI d8", "RST 3",
"RPO", "POP H", "JPO addr", "XTHL", "CPO addr", "PUSH H", "ANI d8", "RST 4", "RPE", "PCHL", "JPE addr", "XCHG", "CPE addr", "*CALL addr", "XRI d8", "RST 5",
"RP", "POP PSW", "JP addr", "DI", "CP addr", "PUSH PSW", "ORI d8", "RST 6", "RM", "SPHL", "JM addr", "EI", "CM addr", "*CALL addr", "CPI d8", "RST 7" };


int LoadFile(const char* file_name, uint8_t** file_data, uint16_t* file_size)
{
	FILE* object_file = fopen(file_name, "rb");
	if (object_file != NULL)
	{
		fseek(object_file, 0, SEEK_END);
		uint64_t size = ftell(object_file);
		if (size < UINT16_MAX)
		{
			fseek(object_file, 0, SEEK_SET);
			*file_size = size;
			*file_data = malloc(*file_size * sizeof(uint8_t));
			if (*file_data != NULL)
			{
				fread(*file_data, sizeof(uint8_t), *file_size, object_file);
				printf("Read %u bytes from file \"%s\"\n\n", *file_size, file_name);
			}
			else
			{
				printf("Couldn't allocate file data\n\n");
				fclose(object_file);
				return 0;
			}
		}
		else
		{
			printf("Couldn't read file \"%s\". File is too big %llu bytes (max size %u bytes)\n\n", file_name, size, UINT16_MAX);
			fclose(object_file);
			return 0;
		}
	}
	else
	{
		printf("Couldn't open file \"%s\"\n\n", file_name);
		return 0;
	}
	fclose(object_file);
	return 1;
}

uint8_t GetCodeLine(uint8_t* object_file, uint16_t* PC, char* code_line)
{
	uint8_t instruction_length = 1;

	uint8_t instruction_byte = object_file[*PC];
	strcpy(code_line, instruction_table[instruction_byte]);
	const char d8[] = "d8";
	const char d16[] = "d16";
	const char a16[] = "addr";

	char* pos = strstr(code_line, d8);
	if (pos != NULL)
	{
		uint8_t data_byte = object_file[++(*PC)];
		char data_hex[5];
		sprintf(data_hex, "#$%02X", data_byte);
		strcpy(pos, data_hex);
		instruction_length = 2;
	}
	else
	{
		pos = strstr(code_line, d16);
		if (pos != NULL)
		{
			uint8_t low_operand = object_file[++(*PC)];
			uint8_t high_operand = object_file[++(*PC)];
			char data_hex[7];
			sprintf(data_hex, "#$%02X%02X", high_operand, low_operand);
			strcpy(pos, data_hex);
			instruction_length = 3;
		}
		else
		{
			pos = strstr(code_line, a16);
			if (pos != NULL)
			{
				uint8_t low_operand = object_file[++(*PC)];
				uint8_t high_operand = object_file[++(*PC)];
				char data_hex[6];
				sprintf(data_hex, "$%02X%02X", high_operand, low_operand);
				strcpy(pos, data_hex);
				instruction_length = 3;
			}
		}
	}

	return instruction_length;
}

uint8_t GetDisassemblyLine(uint8_t* MEM, uint16_t PC, char* print_line)
{
	char code_line[CODE_LINE_LENGTH];
	uint8_t instruction_length = GetCodeLine(MEM, &PC, code_line);

	if (instruction_length == 1)
	{
		sprintf(print_line, "0x%02X %02X             %s ", PC, MEM[PC], code_line);
	}
	else if (instruction_length == 2)
	{
		sprintf(print_line, "0x%02X %02X %02X          %s ", PC - 1, MEM[PC - 1], MEM[PC], code_line);
	}
	else if (instruction_length == 3)
	{
		sprintf(print_line, "0x%02X %02X %02X %02X       %s ", PC - 2, MEM[PC - 2], MEM[PC - 1], MEM[PC], code_line);
	}
	return instruction_length;
}

int Disassemble()
{
	printf("Intel 8080 Disassembler\n");
	while (1)
	{
		int file_read = 0;
		uint8_t* object_file_data = NULL;
		uint16_t file_size = 0;
		char file_name[256];

		while (!file_read)
		{
			TextInputPrompt("Give file name to disassemble: ", file_name);
			file_read = LoadFile(file_name, &object_file_data, &file_size);
		}
		
		uint32_t print_option = 0;
		
		while (1)
		{
			print_option = NumInputPrompt("Print disassembly to:\n1. console\n2. text file\n3. Both\n");

			if (print_option == 1 || print_option == 2 || print_option == 3)
			{
				break;
			}
		}

		char disassembly_file_name[256];
		FILE* disassembly_file = NULL;

		if (print_option == 2 || print_option == 3)
		{
			while (1)
			{
				TextInputPrompt("Give the disassembly text file name: ", disassembly_file_name);
				disassembly_file = fopen(disassembly_file_name, "w");

				if (disassembly_file == NULL)
				{
					printf("Couldn't open file %s", disassembly_file_name);
				}
				else
				{
					break;
				}
			}
		}

		char print_line[DISASSEMBLY_LINE_PREFIX_LENGTH + CODE_LINE_LENGTH];

		uint16_t PC = 0;
		while(PC < file_size)
		{
			uint8_t instruction_length = GetDisassemblyLine(object_file_data, PC, print_line);
			
			if (print_option == 1)
			{
				printf("%s\n", print_line);
			}
			else if (print_option == 2)
			{
				fprintf(disassembly_file, "%s\n", print_line);
			}
			else if (print_option == 3)
			{
				printf("%s\n", print_line);
				fprintf(disassembly_file, "%s\n", print_line);
			}

			PC += instruction_length;
		}

		if (disassembly_file != NULL)
		{
			fclose(disassembly_file);
		}

		free(object_file_data);
		object_file_data = NULL;

		if (print_option == 2 || print_option == 3)
		{
			printf("Wrote disassembly of %s into file %s\n", file_name, disassembly_file_name);
		}

		char answer[256];
		printf("\n");
		while (1)
		{
			TextInputPrompt("Disassemble another file? y/n\n", answer);
			
			if (answer[0] == 'y' || answer[0] == 'n')
			{
				break;
			}
		}

		if (answer[0] == 'n')
		{
			break;
		}
	}
	printf("Exiting disassembler...\n\n");
	return 0;
}
