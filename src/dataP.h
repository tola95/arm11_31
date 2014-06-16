#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char *trim(char *str) {
  char *result = str;
  int i = 0;
  while (str[i] == ' ') {
    result = &str[i + 1];
    i ++;
  }
  return result;
}

//This function takes in the rest of operand2 string and breaks it down into different components and is stored globally in the struct.
void tokenised(Data *ptr, char *operand2) {
  const char delims[3] = ", ";
  char *result;
  result = strtok(operand2, delims);
  ops2->rm = trim(result);         //This is the rm register taken from the operand stored in the struct.
  
  result = strtok(NULL, delims);
  ops2->shiftName = trim(result);  //This is the shiftname taken from the operand and stored in the struct.
  
  result = strtok(NULL, delims);
  ops2->shiftOp = trim(result);    //This is the shiftOp taken from the operand and stored in the struct.
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

enum bool checkForComma(char *ptr) {
  for (int i = 0; i < strlen(ptr); i ++) {
    if (ptr[i] == ','){
    return T;
    }
  }
  return F;
}

uint32_t op2Ror(uint32_t op) {
        uint32_t rorAmt = 0;
        uint32_t curr = op;
        uint32_t mask = 0xc0000000;
        while (curr > 255) {
          uint32_t kj = (mask & curr) >> 30;
          curr <<= 2;
          curr += kj;
          rorAmt ++;
        }
        return (rorAmt <<= 8) + curr;
}

uint32_t getnumber(char *operand2) {
  char *imm = &operand2[1];
  if (operand2[2] == 'x') {
    imm = &operand2[3];
    return strtol(imm, NULL, 16);
  }
  return atoi(imm);
}

uint32_t getRegValue(char *reg) {
        char *p = ((reg) + 1);
        return atoi(p);
}


uint32_t getOp2(char *operand2) {
       uint32_t initial = ALWAYS_COND;      
       if (operand2[0] != 'r') {
           uint32_t number = getnumber(operand2);
           initial+= op2Ror(number);
           initial += (1 << 25);
           return initial;
       }
       if (checkForComma(operand2)) {
           ops2 = malloc(sizeof(Data));
           tokenised(ops2, operand2);
           uint32_t rm = getRegValue(ops2->rm);
           uint32_t shiftT = getShiftType(ops2->shiftName);
           uint32_t shiftby;
           if (ops2->shiftOp[0] == '#') {
               shiftby = getnumber(ops2->shiftOp);
               initial += (shiftby << 7);
           } else {
               initial += (1 << 4);
               initial += ((getRegValue(ops2->shiftOp)) << 8);
           }
           initial += rm;
           initial += (shiftT) << 5;
           return initial;
       }
       uint32_t rm = atoi(operand2 + 1);
       return initial += rm;
}

//This should probably be the main instruction that can be applied to any data processing instruction.   



uint32_t getOpcode(enum mnemonic opcode) {

        switch(opcode) {
            
            case MOV: return (13 << 21);
            case AND: return 0;
            case EOR: return (1 << 21);
            case SUB: return (2 << 21);
            case RSB: return (3 << 21);
            case ADD: return (4 << 21);
            case TST: return (8 << 21);
            case TEQ: return (9 << 21);
            case CMP: return (10 << 21);
            case ORR: return (12 << 21);
            default :
            return 0;
        }
}

uint32_t lsl(enum mnemonic opcode, char *rd, char *rm, char *op1) {
        uint32_t instruction = ALWAYS_COND;        
        instruction += getOpcode(opcode);      
        instruction += (getRegValue(rd) << 12);
        instruction += getRegValue(rm);
        char *endptr;
        char *val = &op1[1];        
        instruction += (uint32_t) (strtol(val, &endptr, 0) << 7); 
        return instruction;
}


//Main (or a different method) checks that opCode and calls this method if the Mnemonic represents
//functions that compute results (and, eor, sub, rsb, add, orr).
uint32_t convertComputable(enum mnemonic opcode, char *rd, char *rn, char *operand2) {
	uint32_t instruction = getOp2(trim(operand2));
	instruction += getOpcode(opcode);
	instruction |= (getRegValue(rd) << 12);     //putting in the rd
	instruction |= (getRegValue(rn) << 16);     //putting in the rn
	return instruction;
}


//Main (or a different method) checks the opCode and calls this method if the Mnemonic is mov.
uint32_t convertMov(enum mnemonic opcode, char *rd, char *operand2) {
	uint32_t instruction = getOp2(trim(operand2));
	instruction += getOpcode(opcode);
	instruction += (getRegValue(rd) << 12);     //putting in the rd
	return instruction;
}


//Main (or a different method) checks the opCode and calls this method if the Mnemonic is represents functions that 
//do not compute results (tst, teq, cmp).
uint32_t convertNonComputable(enum mnemonic opcode, char *rn, char *operand2) {
	uint32_t instruction = getOp2(trim(operand2));
	instruction += getOpcode(opcode);
	instruction |= (getRegValue(rn) << 16);     //putting in the rn
        instruction += (1 << 20);      //setting the s register
	return instruction;
}


