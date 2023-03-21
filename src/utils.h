#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Prompt user for input and read and return one unsigned integer
uint32_t NumInputPrompt(const char* prompt);

// Prompt user for input and read max 255 characters to input_buffer (input_buffer size should be 256)
void TextInputPrompt(const char* prompt, char* input_buffer);

#endif // !UTILS_H