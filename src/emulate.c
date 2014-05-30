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
  {"$0",   0},
  {"$1",   0},
  {"$2",   0},
  {"$3",   0},
  {"$4",   0},
  {"$5",   0},
  {"$6",   0},
  {"$7",   0},
  {"$8",   0},
  {"$9",   0},
  {"$10",  0},
  {"$11",  0},
  {"$12",  0},
  {"SP",   0},
  {"LR",   0},
  {"PC",   0},
  {"CPSR", 0}
};


//enumeration of instruction types.
enum instructionType {
    DATA_PROCESSING, MULTIPLY, SINGLE_DATA_TRANSFER, BRANCH, SPECIAL
} ;


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
        enum bool pending;        
        uint32_t offset;
        uint32_t cond;
        uint32_t op2;
        uint32_t rd;
        uint32_t rn;
        uint32_t rm;
        uint32_t rs;
        uint32_t i;
        uint32_t a;
        uint32_t s;
        uint32_t u;
        uint32_t p;
        uint32_t l;
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
	Rg(rd) = Rg(rm) * Rg(rs);

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
	Rg(rd) = (rm * rs) + rn ;
}
uint32_t ror(uint32_t rmVal, uint32_t shift) {
  return rotate(rmVal, shift);
}

//Branch Instructions
//May or may not be right
//Executed if checkCond(instruction)
void branch(uint32_t offset) {
	offset <<= 2 ;
	PC_ = offset + 8;
	(fetched -> pending) = F;
	(decoded -> pending) = F;
}



// getOp2 uses this function if I is set
uint32_t iIsSet(uint32_t op2) {
  uint32_t imm = masking(op2, 7, 0);
    uint32_t rot = masking(op2, 11, 8);
    return rotate(imm, rot);
}

// getOp2 uses this function if I is not set
uint32_t iIsNotSet(uint32_t op2) {
  uint32_t shift = masking(op2, 11, 4);
  uint32_t rm = op2 & 15;
  uint32_t shiftVal = masking(op2, 11, 7);
  uint32_t shiftType = masking(op2, 6, 5);
  if ((shift & 1) == 1) {
    shiftVal = Rg(masking(op2, 11, 8));
  }
  switch(shiftType) {
    case 0 : return lsl(Rg(rm), shiftVal);
    case 1 : return lsr(Rg(rm), shiftVal);
    case 2 : return asr(Rg(rm), shiftVal);
    case 3 : return ror(Rg(rm), shiftVal);
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

void ldr(uint32_t arg) {

	if (decoded ->l == 1) {

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

	//If P is set, offset is added/subtracted before transferring data
	if (decoded -> p == 1) {
		//If u is set, offset is added
		if (decoded -> u == 1) {
			result = Rg(decoded -> rn) + offset;
		} else {
			result = Rg(decoded -> rn) - offset;
		}
		//Data is transferred
		Rg(decoded->rd) = memPtr[result];

	} else {
		//Data is transferred
		Rg(decoded->rd) = memPtr[offset];
		//If u is set, offset is added
		if (decoded -> u == 1) {
			Rg(decoded -> rn) += offset;
		} else {
			Rg(decoded -> rn) -= offset;
		}
	}

	}
	 
}

void str(uint32_t arg) {

	if (decoded -> l == 0) {
	
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

	//If P is set, offset is added/subtracted before transferring data
	if (decoded -> p == 1) {
		//If u is set, offset is added
		if (decoded -> u == 1) {
			result = Rg(decoded -> rn) + offset;
		} else {
			result = Rg(decoded -> rn) - offset;
		}
		//Data is transferred
		memPtr[result] = Rg(decoded -> rd);

	} else {
		//Data is transferred
		memPtr[offset] = Rg(decoded->rd);
		//If u is set, offset is added
		if (decoded -> u == 1) {
			Rg(decoded -> rn) += offset;
		} else {
			Rg(decoded -> rn) -= offset;
		}
	}

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
        case MLA : ;                 
        case MUL : multiply(decoded->rs, decoded->rm, decoded->rd);
                   break;        
        case LDR : ldr(decoded->offset);
                   break;         
        case STR : str(decoded->offset);
                   break;
        case BEQ : ;
        case BNE : ;
        case BGE : ;
        case BLT : ;
        case BGT : ;
        case BLE : ;
        case B   : branch(decoded->offset);
                   break;         
        default:   break;
  }
    
}

//Helper function to figure out the operation mnemonic from the 32-bit instruction.
enum instructionType mnemonicDecoder(void){

    //Use the bits which are specific to different instruction types to determine
    //The type of instruction.
    
    uint32_t bit27 = masking(fetched->binaryInstruction, 27, 27);
    uint32_t bit26 = masking(fetched->binaryInstruction, 26, 26);
    uint32_t bit25 = masking(fetched->binaryInstruction, 25, 25);
    uint32_t bit07 = masking(fetched->binaryInstruction, 7, 7);
    uint32_t bit04 = masking(fetched->binaryInstruction, 4, 4);

    enum instructionType instructType;
    uint32_t zero = 0;
    uint32_t one  = 1;

    //Algorithm for determining the instruction type from the determining bits.
    //Check Piazza for detailed explanation.
    if (fetched -> binaryInstruction == 0) {

        instructType = SPECIAL;
        
    } else if (bit27 == one){
    
        instructType = BRANCH;
        
    } else if (bit26 == one) {

        instructType = SINGLE_DATA_TRANSFER;
        
    } else if ((bit25 == one) || (bit04 == zero) || (bit07 == 0)) {

        instructType = DATA_PROCESSING;
        
    } else {

        instructType = MULTIPLY;
    }

    //For storing intermediate results.
    enum mnemonic operation;
    uint32_t      opcode;

    switch (instructType) {

        case BRANCH               : opcode  = masking(fetched->binaryInstruction, 31, 28);

            switch(opcode) {

                //Using hex values since binary isnt available.
                
                case 0x0 : operation = BEQ; break; // 0b 0000 == eq
                case 0x1 : operation = BNE; break; // 0b 0001 != ne
                case 0x10: operation = BGE; break; // 0b 1010 >= ge
                case 0x11: operation = BLT; break; // 0b 1011 <  lt
                case 0x12: operation = BGT; break; // 0b 1100 >  gt
                case 0x13: operation = BLE; break; // 0b 1101 <= le
                case 0x14: operation = B;   break; // 0b 1110 T  al
                
            } break;
            
        case MULTIPLY             : opcode = masking(fetched->binaryInstruction, 21, 21);

            switch(opcode) {
            
                case 0: operation = MUL; break;
                case 1: operation = MLA; break;
                
            } break;
        
        case DATA_PROCESSING      : opcode = masking(fetched->binaryInstruction, 24, 21);
        
            switch(opcode) {
            
                //Using hex values since binary isnt available.
                
                case 0x0 : operation = AND; break; // 0b 0000
                case 0x1 : operation = EOR; break; // 0b 0001
                case 0x2 : operation = SUB; break; // 0b 0010
                case 0x3 : operation = RSB; break; // 0b 0011
                case 0x4 : operation = ADD; break; // 0b 0100
                case 0x8 : operation = TST; break; // 0b 1000
                case 0x9 : operation = TEQ; break; // 0b 1001
                case 0x10: operation = CMP; break; // 0b 1010
                case 0x12: operation = ORR; break; // 0b 1100
                case 0x13: operation = MOV; break; // 0b 1101
                
            } break;
        
        case SINGLE_DATA_TRANSFER : opcode = masking(fetched->binaryInstruction, 20, 20) ;

            switch(opcode) {
            
                case 0: operation = STR; break;
                case 1: operation = LDR; break;
                
            } break;

        case SPECIAL              : operation = ANDEQ; break;
        
        
    }

    decoded->operation = operation;
    return instructType;
    
}

//function to decoded a previously fetched instrcution.
void decodeFetchedInstruction(void){
 
  enum instructionType instrType = mnemonicDecoder();
  
  switch(instrType){

        case DATA_PROCESSING :      decoded->cond     = masking(fetched->binaryInstruction, 31, 28) ;
                                    decoded->s        = masking(fetched->binaryInstruction, 20, 20) ;
                                    decoded->i        = masking(fetched->binaryInstruction, 25, 25) ;
                                    decoded->rn       = masking(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rd       = masking(fetched->binaryInstruction, 15, 12) ;
                                    decoded->op2      = masking(fetched->binaryInstruction, 11,  0) ;
                                    break;
        case MULTIPLY :             decoded->cond     = masking(fetched->binaryInstruction, 31, 28) ;
                                    decoded->a        = masking(fetched->binaryInstruction, 21, 21) ;
                                    decoded->s        = masking(fetched->binaryInstruction, 20, 20) ;
                                    decoded->rd       = masking(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rn       = masking(fetched->binaryInstruction, 15, 12) ;
                                    decoded->rs       = masking(fetched->binaryInstruction, 11,  8) ;
                                    decoded->rm       = masking(fetched->binaryInstruction,  3,  0) ;
                                    break;
        case SINGLE_DATA_TRANSFER : decoded->cond     = masking(fetched->binaryInstruction, 31, 28) ;
                                    decoded->i        = masking(fetched->binaryInstruction, 25, 25) ;
                                    decoded->p        = masking(fetched->binaryInstruction, 24, 24) ;
                                    decoded->rn       = masking(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rd       = masking(fetched->binaryInstruction, 15, 12) ;
                                    decoded->u        = masking(fetched->binaryInstruction, 23, 23) ;
                                    decoded->l        = masking(fetched->binaryInstruction, 20, 20) ;
                                    decoded->offset   = masking(fetched->binaryInstruction, 11,  0) ;
                                    break;
        case BRANCH               : decoded->l        = masking(fetched->binaryInstruction, 31, 28) ;
                                    decoded->offset   = masking(fetched->binaryInstruction, 23,  0) ;
                                    break;
        case SPECIAL              : break;

  }
  
  decoded->pending = T;
  
}

//Function to print the state of the memory and registers when the program finishes.
void printFinalState(void){
  printf("Registers: \n");
  for (int i = 0; i<13; i++) {
    printf("%s\t:\t%i (0x%08x) \n", ARMReg[(i)].ident, Rg(i), Rg(i) );
  }
  
  for (int i =15; i<17; i++) {
    printf("%s\t:\t%i (0x%08x) \n", ARMReg[(i)].ident, ARMReg[i].reg, ARMReg[i].reg );
  }

  printf("Non zero memory: \n");
  uint32_t count = 0;
  while(memPtr[count]!=0) {
    printf("0x%08x \t: 0x%08x \n", (count*4), memPtr[count]);
    count++;
  }

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

            if (decoded->operation == ANDEQ) { //check for special halt instruction
                break;
            }

            if ( decoded->cond == CPSR_ ) { //check if conditions match CPSR
                executeDecodedInstruction();
            }
            
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

     printFinalState();
     

        return EXIT_SUCCESS;

 }

 
