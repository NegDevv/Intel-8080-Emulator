#include "disassembler.h"
#include "8080.h"

int main()
{
	uint32_t option = 0;
	while (true)
	{
		printf("Intel 8080 Emulator\n");
		option = NumInputPrompt("1. Emulator\n2. Disassembler\n3. Quit\n");
		printf("\n");

		// Emulator
		if (option == 1)
		{
			InitCPU();

			Run();
		}
		// Disassembler
		else if (option == 2)
		{
			Disassemble();
		}
		// Quit
		else if (option == 3)
		{
			printf("Quitting...");
			break;
		}
	}
	
	return 0;
}