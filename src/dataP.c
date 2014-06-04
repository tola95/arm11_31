#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//This constant represents an incomplete instruction that is executed ALWAYS. (1110) as the cond. 
const static uint32_t ALWAYS_COND = 0xe0000000;


//
typedef struct deconstruct {
  char *rm;
  char *shiftName;
  char *shiftOp;
} Data;

//To initialise and allocate space for data struct
Data *ops2;


//This function takes in the rest of operand2 string and breaks it down into different components and is stored globally in the struct.
void tokenised(Data *ptr, char *operand2) {
  const char delims[3] = ", ";
  char *result;
  result = strtok(operand2, delims);
  ops2->rm = result;         //This is the rm register taken from the operand stored in the struct.
  
  result = strtok(NULL, delims);
  ops2->shiftName = result;  //This is the shiftname taken from the operand and stored in the struct.
  
  result = strtok(NULL, delims);
  ops2->shiftOp = result;    //This is the shiftOp taken from the operand and stored in the struct.
}


//This function is called to return the binary form of lsl, lsr, ror, asr.
uint32_t getShiftType(char *ptr) {
  if (strcmp(ptr, "lsl") == 0) {
    return 0;  
  }
  if (strcmp(ptr, "lsr") == 0) {
    return 1;  
  }
  if (strcmp(ptr, "asr") == 0) {
    return 2;
  }
  return 3;                  //assuming that the shiftType will always be lsl, lsr, asr, ror.
}


//This method is called by every function to get the initial form of the instruction(ALWAYS_COND) and to check
//whether the operand2 was an immediate value or a shifted register. 
uint32_t getOp2(char *operand2) {
	uint32_t initial = ALWAYS_COND;
	if (operand2[0] == '#') {
		char *imm = &operand2[1];
		uint32_t number = atoi(imm);
		initial += number;
		initial += (1 << 25);  // 25th bit is set to let the emulator know that op2 is an immediate value;
	}
	ops2 = malloc(sizeof(Data));
	tokenised(ops2, operand2);
	char *ptr = ((ops2->rm) + 1);
	uint32_t rm = atoi(ptr);
	uint32_t shiftT = getShiftType(ops2->shiftName);
	uint32_t shiftby;
	if (ops2->shiftOp[0] == '#') {
	  char *p = ((ops2->shiftOp) + 1);
          shiftby = atoi(p);
          initial += (shiftby << 7);
	} else {
	  char *reg = ((ops2->shiftOp) + 1);
	  shiftby = atoi(reg);
	  initial += (shiftby << 8);
	  initial += (1 << 4);
	}
	initial += rm;
	initial += (shiftT) << 5;
	return initial;
}


/*
//Main (or a different method) checks that opCode and calls this method if the Mnemonic represents
//functions that compute results (and, eor, sub, rsb, add, orr).
uint32_t convertComputable(uint32_t opCode, uint32_t rd, uint32_t rn, char *operand2) {
	uint32_t instruction = getOp2(operand2);
	instruction |= (opCode << 24); //putting in the opcode
	instruction |= (rd << 12);     //putting in the rd
	instruction |= (rn << 16);     //putting in the rn
	return instruction;
}


//Main (or a different method) checks the opCode and calls this method if the Mnemonic is mov.
uint32_t convertMov(uint32_t opCode, uint32_t rd, char *operand2) {
	uint32_t instruction = getOp2(operand2);
	instruction |= (opCode << 24); //putting in the opcode
	instruction |= (rd << 12);     //putting in the rd 
	return instruction;
}


//Main (or a different method) checks the opCode and calls this method if the Mnemonic is represents functions that 
//do not compute results (tst, teq, cmp).
uint32_t convertNonComputable(uint32_t opCode, uint32_t rn, char *operand2) {
	uint32_t instruction = getOp2(operand2);
	instruction |= (opCode << 24); //putting in the opcode
	instruction |= (rn << 16);     //putting in the rd
	return instruction;
}
*/

//This should probably be the main instruction that can be applied to any data processing instruction.   

uint32_t convertDataProcessInst(char *opCode, uint32_t rd, uint32_t rn  , char *operand2) {
        uint32_t instruction = getOp2(operand2);
        instruction |= (opCode << 24); //putting in the opcode
        instruction |= (rd << 12);     //putting in the rd
	instruction |= (rn << 16);     //putting in the rn
	return instruction;
}





//Dummy main for testing.
int main() {
        ops2 = malloc(sizeof(Data));
        char str[] = "This is a, sentence.";
        printf("%s\n", str);
        char delims[3] = ", ";
        char *ptr = strtok(str, delims);
        printf("%s\n", ptr);
        ops2->rm = ptr;
        ptr = strtok(NULL, delims);
        ops2->shiftName = ptr;
        printf("%s\n", ptr);
        char delim[1] = "";
        ptr = strtok(NULL, delim);
        printf("%s\n", ptr);
	return 0;
	
}


