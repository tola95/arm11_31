/*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h> 

//These macros make it easier to reference the registers, leading to prettier code.
#define Rg(X)  ARMReg[(X)].reg
#define SP_    ARMReg[13].reg
#define LR_    ARMReg[14].reg
#define PC_    ARMReg[15].reg
#define CPSR_  ARMReg[16].reg

 //Since static const didn't work, this is an alternative 
//which is safer than just writing the number.
enum {REG = 17, MEM = 16384};
enum bool {F, T};

/*typedef only gives name to the struct ARM_STATE so that 
you don't have to write struct ARM_STATE when you initialise
the struct. This leads you to only have to write State when 
initialising*/

//creating struct for each register.
typedef struct ARM_REGISTER {
  char ident[4];
  uint32_t reg;
} Register;

//Creating a pointer to the ARM Machine main memory.
uint32_t *memPtr;

//This method is called by the main method to initialise all the memory.
void initMem() {
  memPtr = (uint32_t *) calloc(MEM, sizeof(uint32_t));

} 

//Initialised registers to 0 and assigned each
//register to its string name.
Register ARMReg[REG] = {
  {"$0", 0},
  {"$1", 0},
  {"$2", 0},
  {"$3", 0},
  {"$4", 0},
  {"$5", 0},
  {"$6", 0},
  {"$7", 0},
  {"$8", 0},
  {"$9", 0},
  {"$10", 0},
  {"$11", 0},
  {"$12", 0},
  {"SP", 0},
  {"LR", 0},
  {"PC", 0},
  {"CPSR", 0}
};

//enumeration of all operation mnemonics for help with decoding and execution.
enum mnemonic {  AND, EOR, SUB,
                 RSB, ADD, TST,
                 TEQ, CMP, ORR,
                 MOV, MUL, MLA,
                 LDR, STR, BEQ,
                 BNE, BGE, BLT,
                 BGT, BLE, B,
                 LSL, ANDEQ };

//Struct representing decoded instruction.
struct decodedInstruction {
        enum mnemonic operation;
        uint32_t rd;
        uint32_t rn;
        uint32_t rm;
        uint32_t op2;
        uint32_t i;
        uint32_t a;
        uint32_t s;
	uint32_t p;
	uint32_t l;
	uint32_t u;
        uint32_t cond;
        enum bool pending;
} ;
    
//Struct representing fetched instruction.
struct fetchedInstruction {
        uint32_t binaryInstruction;
        enum bool pending;
} ;
    
struct decodedInstruction *decoded;
struct fetchedInstruction *fetched;

void initdf(void) {
      decoded = malloc(sizeof(struct decodedInstruction));
      fetched = malloc(sizeof(struct fetchedInstruction));    
}

void printBits1(uint32_t x) { 
  int i;
  uint32_t mask = 1 << 31;
  for(i=0; i<32; ++i) { 
    if((x & mask) == 0)
      printf("0"); 
    else
      printf("1"); x = x << 1;
  }
  printf("\n "); }

//Data Processing instruction opcodes.

/*  These use macros defined earlier to make code easier to read.
    Rg(x) is replaced with ' ARMReg[x].reg ' at compile time.
    PC_,SD_,LR & CPSR_ are similarly replaced with their respective registers.
*/


void and(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) & op2;
}

void eor(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) ^ op2;
}

void sub(uint32_t rn, uint32_t op2, uint32_t rd) {
  uint32_t one = 1;
  Rg(rd) = Rg(rn) + (~op2 + one);
}

void rsb(uint32_t rn, uint32_t op2, uint32_t rd) {
  uint32_t one = 1;
  Rg(rd) = op2 + (~Rg(rn) + one);
}

void add(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) + op2;
}

uint32_t tst(uint32_t rn, uint32_t op2) {
  uint32_t result = Rg(rn) & op2;
  return result;
}

uint32_t teq(uint32_t rn, uint32_t op2) {
  uint32_t result = Rg(rn) ^ op2;
  return result;
} 

uint32_t cmp(uint32_t rn, uint32_t op2) {
  uint32_t one = 1;
  uint32_t result = Rg(rn) + (~op2 + one);
  return result;
}

void orr(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) | op2;
}

void mov(uint32_t op2, uint32_t rd) {
  Rg(rd) = op2;
}

//Creates a mask given the first and last index.
uint32_t masking(uint32_t inst, int left, int right) {
  int m = ((left - right) + 1);
  uint32_t result = ~(~0 << m) << right;
  inst &= result;
  inst >>= right;
  return inst;
}

uint32_t rotate(uint32_t imm, uint32_t n) {
  while (n > 0) {
    uint32_t zerobit = 1 & imm;
    zerobit <<= 31;
    imm >>= 1;
    imm += zerobit;
    n --;
  }
  return imm;
}

//Function to check conditions
enum bool checkCond(uint32_t instruction) {
	uint32_t cond = masking(instruction, 31, 28 );
	uint32_t cpsrCond = masking(ARMReg[16].reg, 31, 28);
	 if ((cond == cpsrCond) || cond == 14) {return T;} //Code = 1110 or matches CPSR
	else {return F;}
}


//Helper method to change a specific bit
 uint32_t changebit(uint32_t i, int bit, int change) {

	assert(change == 0 || change == 1);

	uint32_t mask = 1 * pow(2, bit);

	if (change == 1) {
		i |= mask; 
	} else if (change == 0) {
		i &= ~mask;
	} 

	return i;
}


//Opcode and operation for when operand2 is not an immediate constant.
uint32_t lsl(uint32_t rm, uint32_t shift) {
  return rm << shift;
}

uint32_t lsr(uint32_t rm, uint32_t shift) {
  return rm >> shift;
}

//Multiply Instructions

void multiply(uint32_t rs, uint32_t rm, uint32_t rd) { 
	ARMReg[rd].reg = rm * rs;

//	if () { //S bit is set
		//N and Z bits of CPSR should be updated
//	}
}

uint32_t asr(uint32_t rmVal, uint32_t shift) {
  uint32_t bit31 = rmVal & (1 << 31);
  while (shift > 0) {
    rmVal >>= 1;
    rmVal += bit31;
    shift --;
  }
  return rmVal;
}
void multiplyacc(uint32_t rs, uint32_t rm, uint32_t rd, uint32_t rn) {
	ARMReg[rd].reg = (rm * rs) + rn ;
}
uint32_t ror(uint32_t rmVal, uint32_t shift) {
  return rotate(rmVal, shift);
}

//Branch Instructions
void branch(uint32_t offset) {
	offset <<= 2 ;
	PC_ = offset + 8;
	(fetched-> pending) = F;
	(decoded -> pending) = F;
}


// getOp2 uses this function if I is set
uint32_t iIsSet(uint32_t op2) {
  uint32_t imm = masking(op2, 7, 0);
    uint32_t rot = masking(op2, 11, 8);
    return rotate(imm, rot);
}

// getOp2 uses this function if I is set
uint32_t iIsNotSet(uint32_t op2) {
  uint32_t shift = masking(op2, 11, 4);
  uint32_t rm = op2 & 15;
  uint32_t shiftVal = masking(op2, 11, 7);
  uint32_t shiftType = masking(op2, 6, 5);
  if ((shift & 1) == 1) {
    shiftVal = ARMReg[masking(op2, 11, 8)].reg;
  }
  switch(shiftType) {
    case 0 : return lsl(ARMReg[rm].reg, shiftVal);
    case 1 : return lsr(ARMReg[rm].reg, shiftVal);
    case 2 : return asr(ARMReg[rm].reg, shiftVal);
    case 3 : return ror(ARMReg[rm].reg, shiftVal);
    default:
    return 0;
  }
}

uint32_t getOp2(void) {
  uint32_t operand2 = decoded->op2;
  switch(decoded->i) {
    case 0: return iIsNotSet(operand2);
    case 1: return iIsSet(operand2);
    default:
    return 0;
  }
}

//Single Data Transfer Instructions
void preIndex(uint32_t arg) { //If P is set
	uint32_t offset = 0;
	uint32_t result = 0;
	//If I is set, arg is interpreted as a shift register, otherwise 12 bit offset
	switch (decoded -> i) {
		case 1 : offset = iIsSet(arg);
		break;
		case 0 : offset = arg;
		break;
		default : ;
		break;
	} 
	//If U is set, offset is added to contents of base register. Subtracted otherwise
	switch (decoded -> u) {
		case 1 : result = Rg(decoded -> rn) + offset; 
		break;
		case 0 : result = Rg(decoded -> rn) - offset;
		break;
		default : ;
		break;
	}
	//If L is set word is loaded from memory, stored to memory otherwise
	switch (decoded -> l) {
		case 1 : Rg(decoded -> rd) = memPtr[result];
		break;
		case 0 : memPtr[result] = Rg(decoded -> rd);
		break;
		default : ;
		break;
	}
}

void postIndex(uint32_t arg) { //If P is not set
	uint32_t offset = 0;

	//If I is set, arg is interpreted as a shift register, otherwise 12 bit offset
	switch (decoded -> i) {
		case 1 : offset = iIsSet(arg);
		break;
		case 0 : offset = arg;
		break;
		default :
		break;
	} 
	//If L is set word is loaded from memory, stored to memory otherwise
	switch (decoded -> l) {
		case 1 : Rg(decoded->rd) = memPtr[offset];
		break;
		case 0 : memPtr[offset] = Rg(decoded->rd);
		break;
		default :
		break;
	}
	//If U is set, offset is added to contents of base register. Subtracted otherwise
	switch (decoded -> u) {
		case 1 : Rg(decoded->rn) = Rg(decoded->rn) + offset;
		break;
		case 0 : Rg(decoded->rn) = Rg(decoded->rn) - offset;
		break;
		default :
		break;
	}
} 
 

//function to increment PC to next instruction address.
void incrementPC(void){

    PC_ += 1;//PC_ is a macro defined earlier

}


//function to fetch the instruction at the current instruction address stored in PC.
void fetchNextInstruction(void){

    fetched->binaryInstruction = PC_; //PC_ is a macro defined earlier.
    fetched->pending = T;
    
}
 
//function to execute decoded instructions and clear decoded instructions pending flag.
void executeDecodedInstruction(void){


    decoded->pending = F;
    
    enum mnemonic op = decoded->operation;
    
    switch (op){
    
        case AND : and(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case EOR : eor(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case SUB : sub(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case RSB : rsb(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case ADD : add(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case TST : tst(decoded->rn, decoded->op2);
                   break;
        case TEQ : teq(decoded->rn, decoded->op2);
                   break;
        case CMP : cmp(decoded->rn, decoded->op2);
                   break;
        case ORR : orr(decoded->rn, decoded->op2, decoded->rd);
                   break;
        case MOV : mov(decoded->op2, decoded->rd);
                   break;
        default:   break;
  }
    
}

//Helper function to figure out the operation mnemonic from the 32-bit instruction.
enum mnemonic mnemonicDecoder(void){
    return AND;
}

//function to decoded a previously fetched instrcution.
void decodeFetchedInstruction(void){
 
  decoded->operation = mnemonicDecoder();
  
  switch(decoded->operation){

        case AND : ;
        case EOR : ;
        case SUB : ;
        case RSB : ;
        case ADD : ;
        case TST : ;
        case TEQ : ;
        case CMP : ;
        case ORR : ;
        case MOV : ; break;
        
        default:   break;

  }
  
  decoded->pending = T;
  
}



 //Main Function
 int main(int argc, char **argv) {

     initMem();
     initdf();

     /* file loading */
     FILE *file = fopen(argv[1], "rb");

     if (file == NULL){
             perror( " Error: Could not open file. \n" );
             exit(EXIT_FAILURE);
     }
     
     
     /* Calculate file size and then from the size, the number of 32-bit instructions in the file */
     const int bytesPerInstruction = 4;
     fseek( file, 0, SEEK_END );
     int instructionsSize = ftell(file)/bytesPerInstruction;
     fseek( file, 0, SEEK_SET );

     //Load the binary data into the memory array.
     fread(memPtr, bytesPerInstruction, instructionsSize, file );

     /* file closing */
     fclose(file);



     //Pipeline execution of instructions.
    
     //main pipeline loop.
     while ( T ) { //Loops infinitely until halt instruction is called



        if ( decoded->pending == T ) {

            executeDecodedInstruction();
            decoded->pending = F;

        }

        if ( fetched->pending == T ) {

            decodeFetchedInstruction();
            fetched->pending = F;
            
        }

        if (PC_ <  MEM) { // PC_ is a macro defined earlier.

            fetchNextInstruction();
            fetched->pending = T;
            
        } else if (decoded->pending == F) {
        
            break;
            
        }


        incrementPC();

     
     }
     

         return EXIT_SUCCESS;

 }