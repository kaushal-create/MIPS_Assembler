# MIPS_Assembler

/*****************************************************************************
TITLE: Claims																																
AUTHOR: Kaushal Raj (2001CS36)
Declaration of Authorship
This txt file, claims.txt, is part of the assignment of CS322 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

Files:
==========
*All the files i submitted, asm.c++ and claims.txt, have my name and student id at the start, along with a declaration of authorship.

The evidence for Assembler including the output files from the test examples has also been uploaded. They are test1(3 output files), test2(3 output files), test3
(3 output file), bubble(3 output files), test4(3 output file), test5(3 output file), bubble.txt and test_asm(3 output files), test_asm.txt. 


* Programs are compilable with g++ assembler.cpp -o asm
* Running the program ./asm bubble.asm
* Programs are compilable with g++ emu.cpp -o emu
* Running the emulatior ./emu bubble.o


# Program format and structure:
1. The assembler:

*	Can handle extra spaces between operands and extra whitespaces except labels where providing space is neccessary after it's declarations. 

*	Assembler can detect the following errors:
	- Bogus mnemonic
	- Incorrect label naming format
	- Repeated label
	- Label not found
	- Label name can not be a keyword
	- Missing operand
	- More operands than expected
	- Invalid operand format
	- Incorret label format 

*	Assembler can issue the following warnings:
	- Unused label
	- Integer overflow w.r.t signed 32- bit

*	Stores mnemonic and their opcodes.
*	Stores defined labels in program.
*	Uses single routine for both pases to assemble the program.
*	Generates an advanced listing file.
*	Generates an object file with .o extension
*	Listing and object files are empty if there are any errors.
*	Can implement the instruction SET.

2. The emulator: 

*	My emulator is both interactive and log based. You can run each instruction line by line or output all the trace directly into file_name.txt

*	Emulator can do following things:
  - Memory Dump using instruction: -dump
  - Emulate code one line at time: using instruction: -t
  - Emulate till the end using: -run
  - Show registers and SP values: -reg
  - Show instruction set using: -isa
  - Read operations using: -read
  - Write operations using: -write

*	Unique Feature:
	- Tells next instruction to be executed that can help in debugging.
	- Is interactive
	- Is very flexible with input and error handling
	- For e.g. if given wrong input during addressing dump it asks again for the correct format
	- Convenient dumping format asks for base and offset so doesn't overflows the screen.

*	Can do other normal things in instruction as well.	
*	Loads Object file
*	Produces memory dump
*	execute test program
*	Detects errant programs


3. Bubble sort: 

*	Assemble
*	Works

# Testing:
1. The assembler:

I have tested five examples shown as bellow. The errors only occurred in the file test2.asm and 
the program immediately stopped on the second pass. 

The others were tested without any errors detected and produced three output files, listing file, 
error log file and machine readable object file, for each example. 

After comparing the listing files and object file of memcpy.asm , memcpy.l, triangle.l memcpy.o 
with the implemented result given on the assignment, both of them show no difference 
from those provided( if there is different  that is fine too) 

1)
- Input: ./asm test1.asm
- Output: 
a. test1.l 
b. test1.log (without any error messages)
c. test1.o

2)
- Input: ./asm test2.asm
- Output: 
b. test2.log (with 9 error messages shown as below but also in log file)

error messages:
Error at line no. : 4 -- Type: Multiple declaration of label: label
Error at line no. : 5 -- Type: No such label / data variable
Error at line no. : 6 -- Type: Invalid number
Error at line no. : 7 -- Type: Invalid OPCode-Syntax combination
Error at line no. : 8 -- Type: Invalid OPCode-Syntax combination
Error at line no. : 9 -- Type: Invalid OPCode-Syntax combination
Error at line no. : 10 -- Type: Invalid label name
Error at line no. : 11 -- Type: Invalid Mnemonic
Error at line no. : 12 -- Type: Invalid Mnemonic
Object and Listing file is not created here due to compilation errors.

3)
- Input: ./asm test3.asm
- Output: 
a. test3.l 
b. test3.log (without any error messages)
c. test3.o


4)
- Input: ./asm sumN.asm
- Output: 
a. sumN.l 
b. sumN.log (without any error messages)
c. sumN.o

5)
- Input: ./asm triangle.asm
- Output: 
a. triangle.l 
b. triangle.log (without any error messages)
c. triangle.o

2. The emulator:
 
1)
- Input: ./emu -trace test1.o 
- Output: 
manually aborted (infinite loop)

2)
- Input: ./emu -trace test2.o
- Output: 
Error : No such file exists

3)
- Input: ./emu triangle.o, -run
- Output:
No Error
HALT found
47654 Statements were executed total

# Bubble Sort Program :
- Input: ./emu -t bubble_sort.o
- Output:
No Error
HALT found
1044 Statements were executed total
