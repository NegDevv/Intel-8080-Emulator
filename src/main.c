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
			uint32_t emulator_option = 0;

			InitCPU();

			while (true)
			{
				emulator_option = NumInputPrompt("1. Run\n2. Configure\n3. Exit\n");

				if (emulator_option == 1)
				{
					Run();
					InitCPU(); // Reset CPU to default values after run
				}
				else if (emulator_option == 2)
				{
					Configure();
				}
				else if (emulator_option == 3)
				{
					break;
				}
			}
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