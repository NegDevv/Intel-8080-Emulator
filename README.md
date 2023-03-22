# Intel 8080 Emulator
 
 Emulator and disassembler for the 8-bit Intel 8080 microprocessor.

 The goal of this project is to create accurate enough emulation of the Intel 8080 and necessary arcade hardware to play the original arcade version of Space Invaders (1978).

 ## Progress

 - [x] Disassembler
 - [x] All instructions implemented
 - [x] Program loader
 - [] Proper CPU emulation verified
 - [] Debug GUI?
 - [x] Step by step program execution
 - [] I/O hardware implemented

 ## Usage

 The disassembler is working and can be tested with the provided test ROMs in the [tests](tests/) folder.

Example:

 1. Build and run the Visual Studio Solution
 2. Press 2 to select the disassembler and hit enter
 3. Enter filename e.g. tests/cputest.com and hit enter
 4. Select print option and hit enter

    4.1 Enter disassembly text file name and hit enter

 5. Press y or n to dissassemble another file and hit enter

```
Intel 8080 Emulator
1. Emulator
2. Disassembler
3. Quit
2

Intel 8080 Disassembler
Give file name to disassemble: tests/cputest.com
Read 19200 bytes from file "tests/cputest.com"

Print disassembly to:
1. console
2. text file
3. Both
3
Give the disassembly text file name: cputest_dis.txt

0x00 3E 02          MVI A,#$02
0x02 3C             INR A
0x03 EA 00 30       JPE $3000
0x06 C3 09 01       JMP $0109
0x09 F3             DI
0x0A 31 FF 2F       LXI SP,#$2FFF
0x0D CD 4F 21       CALL $214F
0x10 CD 4E 0B       CALL $0B4E
0x13 0D             DCR C
0x14 0A             LDAX B
0x15 44             MOV B,H
0x16 49             MOV C,C
0x17 41             MOV B,C
0x18 47             MOV B,A
0x19 4E             MOV C,M
0x1A 4F             MOV C,A
0x1B 53             MOV D,E
0x1C 54             MOV D,H
0x1D 49             MOV C,C
0x1E 43             MOV B,E
0x1F 53             MOV D,E
0x20 20             *NOP
0x21 49             MOV C,C
0x22 49             MOV C,C
0x23 20             *NOP
0x24 56             MOV D,M
0x25 31 2E 32       LXI SP,#$322E
0x28 20             *NOP
0x29 2D             DCR L
0x2A 20             *NOP
0x2B 43             MOV B,E

...

0x4AFA 69             MOV L,C
0x4AFB CD 8B 0B       CALL $0B8B
0x4AFE CD 6C FD       CALL $FD6C

Wrote disassembly of tests/cputest.com into file cputest_dis.txt

Disassemble another file? y/n
n
Exiting disassembler...

```

 *note: every byte is interpreted as a part of an instruction, thus bytes representing data can show as confusing code segments in the disassembly. Illegal opcodes are marked with "\*" .*


