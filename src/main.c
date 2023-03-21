#include "disassembler.h"
#include "8080.h"

int main()
{
	int option = 0;
	while (true)
	{
		printf("Intel 8080 Emulator\n");
		printf("1. Emulator\n2. Disassembler\n3. Quit\n");
		scanf_s("%1d", &option);
		fseek(stdin, 0, SEEK_END);
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
		else
		{
			printf("Invalid input!\n\n");
		}
	}
	

	return 0;
}