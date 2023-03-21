#include "utils.h"
#include <stdio.h>

// Prompt user for input and read and return one unsigned integer
uint32_t NumInputPrompt(const char* prompt)
{
	uint32_t num;
	printf(prompt);
	if (!(scanf_s("%u", &num) == 1))
	{
		num = 0;
		printf("Invalid input!\n\n");
	}
	fseek(stdin, 0, SEEK_END);
	return num;
}

// Prompt user for input and read max 255 characters to input_buffer (input_buffer size should be 256)
void TextInputPrompt(const char* prompt, char* input_buffer)
{
	printf(prompt);
	scanf_s("%255s", input_buffer, 256);
	fseek(stdin, 0, SEEK_END);
}
