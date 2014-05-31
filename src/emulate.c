
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
enum {REG = 17, MEM = 65536};
enum bool {F, T};

//creating struct for each register.
typedef struct ARM_REGISTER {
  char ident[4];
  uint32_t reg;
} Register;

//Creating a pointer to the ARM Machine main memory.
uint8_t *memPtr;

//This method is called by the main method to initialise all the memory.
void initMem() {
  memPtr = (uint8_t *) calloc(MEM, sizeof(uint8_t));

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
        uint32_t rs;
        uint32_t rm;
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

uint32_t fetchFromMem(int start) {

  uint32_t reversedInst = 0;

  for (int i = 0; i < 4; i++) {

    uint32_t val = memPtr[start + i] << i * 8;
    reversedInst |= val;

  }

  return reversedInst;

}

void putInMem(uint32_t memAddr, uint32_t value){
  uint32_t val = 0;
  uint32_t charmask = (0xff);
  for (int i = 3; i > 0; i --) {
    val = (value & charmask) << i * 8;
    charmask <<= i * 6;
  }
  memPtr[memAddr] = val;

}


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

// Since V bit is unaffected, this funtion preserves the 28th bit of the CPSR
    //and initialises eveything else to zero
uint32_t setCPSRA() {
  CPSR_ &= (1 << 28);
  return CPSR_;
}

void setCPSRL(uint32_t rd) {
  if (decoded->s == 1) {
    CPSR_ = setCPSRA();
    if (Rg(rd) == 0) {
      CPSR_ += (1 << 30);
    }
    if ((Rg(rd) & (1 << 31)) == pow(2, 31)) {
      CPSR_ += 1 << 31;
    }

  }
}


void and(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) & op2;
  if (decoded->s == 1) {
    setCPSRL(rd);
  }
}

void eor(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) ^ op2;
  if (decoded->s == 1) {
    setCPSRL(rd);
  }
}

void sub(uint32_t rn, uint32_t op2, uint32_t rd) {
  uint32_t one = 1;
  Rg(rd) = Rg(rn) + (~op2 + one);
  if (decoded->s == 1) {
    CPSR_ = setCPSRA();
    if (Rg(rd) == 0) {
      CPSR_ += (1 << 30);
    }
    if ((Rg(rd) & 1 << 31) == pow(2, 31)) {
      CPSR_ += 1 << 31;
    }
    if (op2 > Rg(rn)) {
      CPSR_ += 1 << 29;
    }
  }
}

void rsb(uint32_t rn, uint32_t op2, uint32_t rd) {
  uint32_t one = 1;
  Rg(rd) = op2 + (~Rg(rn) + one);
  if (decoded-> s == 1) {
    CPSR_ = setCPSRA();
    if (Rg(rd) == 0) {
      CPSR_ += (1 << 30);
    }
    if ((Rg(rd) & 1 << 31) == pow(2, 31)) {
      CPSR_ += 1 << 31;
    }
    if (Rg(rn) > op2) {
      CPSR_ += 1 << 29;
    }
  }
}

void add(uint32_t rn, uint32_t op2, uint32_t rd) {
  Rg(rd) = Rg(rn) + op2;
  uint64_t carryChecker = Rg(rn) + op2;
  if (decoded->s == 1) {
    setCPSRL(rd);
    if (carryChecker > ( pow(2,32) - 1 )){
        CPSR_ += 1 << 29;
    }
  }
}

uint32_t tst(uint32_t rn, uint32_t op2) {
  uint32_t result = Rg(rn) & op2;
  return result;
 //////////// setCPSRL(rd);
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
uint32_t getVal(uint32_t inst, int left, int right) {
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


//Opcode and operation for when operand2 is not an immediate constant.
uint32_t lsl(uint32_t rm, uint32_t shift) {
  //uint32_t carryOut = rm << shift - 1;
  uint32_t get = 31 - (shift - 1);
  if (decoded->s == 1)  {
    CPSR_ += ( (getVal(rm, get, get)) << (shift - 1)) >> 2;
   // CPSR_ += (getVal(rm, 31, 31)) << 29; 
  }
  return rm << shift;
}

uint32_t lsr(uint32_t rm, uint32_t shift) {
  return rm >> shift;
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

uint32_t ror(uint32_t rmVal, uint32_t shift) {
  if (decoded->s == 1) {
  CPSR_ += (getVal(rmVal, shift - 1, shift - 1)) >> (shift -1);
  }
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
  CPSR_ = setCPSRA();
  //CPSR_ += (getVal(op2, 3, 3)) << 29;
  uint32_t imm = getVal(op2, 7, 0);
    uint32_t rot = getVal(op2, 11, 8);
    return rotate(imm, rot);
}

// getOp2 uses this function if I is not set
uint32_t iIsNotSet(uint32_t op2) {

  CPSR_ = setCPSRA();
  uint32_t shift = getVal(op2, 11, 4);
  uint32_t rm = op2 & 15;
  uint32_t shiftVal = getVal(op2, 11, 7);
  uint32_t shiftType = getVal(op2, 6, 5);
  if ((shift & 1) == 1) {
    shiftVal = ARMReg[getVal(op2, 11, 8)].reg;
  }
  switch(shiftType) {
    case 0 : 
	     return lsl(Rg(rm), shiftVal);
   	     break;
    case 1 : if (decoded->s == 1) CPSR_ += (getVal(rm, 3, 3)) << 29;
  	     return lsr(Rg(rm), shiftVal);
             break;
    case 2 : if (decoded->s == 1) CPSR_ += (getVal(rm, 3, 3)) << 29;
             return asr(Rg(rm), shiftVal);
             break;
    case 3 : 
             return ror(Rg(rm), shiftVal);
             break;
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
                Rg(decoded->rd) = fetchFromMem(result);

        } else {
                //Data is transferred
                Rg(decoded->rd) = fetchFromMem(offset);
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
                putInMem((result) , Rg(decoded -> rd));

        } else {
                //Data is transferred
                putInMem((offset) , Rg(decoded->rd));
                //If u is set, offset is added
                if (decoded -> u == 1) {
                        Rg(decoded -> rn) += offset;
                } else {
                        Rg(decoded -> rn) -= offset;
                }
        }

        }
        
}

       
 

void multiply(void) {
  uint32_t result = Rg(decoded->rm) * Rg(decoded->rs);
  if (decoded->a == 1) {
    result += Rg(decoded->rn);
  }
  Rg(decoded->rd) = result;
  if (decoded->s == 1) {
    CPSR_ = 0;
    if (result == 0) {
      CPSR_ += (1 << 30);
    }
    CPSR_ += (result & (1 << 31));
  }
}

//function to increment PC to next instruction address.
void incrementPC(void){

    PC_ += 4;//PC_ is a macro defined earlier

}

int checkFlag(char flag) {
  uint32_t mask;
  switch (flag) {
    case 'N' : mask = 0x80000000; break;
    case 'Z' : mask = 0x40000000; break;
    case 'C' : mask = 0x20000000; break;
    case 'V' : mask = 0x10000000; break;
    default:
    return 0;
  }
  return (CPSR_ & mask) != 0;
}

//Function to check conditions
int checkCond() {
  switch (decoded->cond) {
    case 0:
      return checkFlag('Z');
    case 1:
      return checkFlag('Z') == 0;
    case 10:
      return checkFlag('N') == checkFlag('V');
    case 11:
      return checkFlag('N') != checkFlag('V');
    case 12:
      return checkFlag('Z') == 0 && (checkFlag('N') == checkFlag('V'));
    case 13:
      return checkFlag('Z') | (checkFlag('N') != checkFlag('V'));
    case 14:
      return 1;
    default :
      return 0;
  }
}


//function to fetch the instruction at the current instruction address stored in PC.
void fetchNextInstruction(void){

    fetched->binaryInstruction = fetchFromMem(PC_); //PC_ is a macro defined earlier.
    fetched->pending = T;
   
}
 
//function to execute decoded instructions and clear decoded instructions pending flag.
void executeDecodedInstruction(void){


    decoded->pending = F;
    enum mnemonic op = decoded->operation;
   
    switch (op){
   
        case AND : and(decoded->rn, getOp2(), decoded->rd);
                   break;
        case EOR : eor(decoded->rn, getOp2(), decoded->rd);
                   break;
        case SUB : sub(decoded->rn, getOp2(), decoded->rd);
                   break;
        case RSB : rsb(decoded->rn, getOp2(), decoded->rd);
                   break;
        case ADD : add(decoded->rn, getOp2(), decoded->rd);
                   break;
        case TST : tst(decoded->rn, getOp2());
                   break;
        case TEQ : teq(decoded->rn, getOp2());
                   break;
        case CMP : cmp(decoded->rn, getOp2());
                   break;
        case ORR : orr(decoded->rn, getOp2(), decoded->rd);
                   break;
        case MOV : mov(getOp2(), decoded->rd);
                   break;
        case MLA : ;                
        case MUL : multiply();
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
   
    uint32_t bit27 = getVal(fetched->binaryInstruction, 27, 27);
    uint32_t bit26 = getVal(fetched->binaryInstruction, 26, 26);
    uint32_t bit25 = getVal(fetched->binaryInstruction, 25, 25);
    uint32_t bit07 = getVal(fetched->binaryInstruction, 7, 7);
    uint32_t bit04 = getVal(fetched->binaryInstruction, 4, 4);

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

        case BRANCH               : opcode  = getVal(fetched->binaryInstruction, 31, 28);

            switch(opcode) {

                //Using hex values since binary isnt available.
               
                case 0x0 : operation = BEQ; break; // 0b 0000 == eq
                case 0x1 : operation = BNE; break; // 0b 0001 != ne
                case 0xA : operation = BGE; break; // 0b 1010 >= ge
                case 0xB : operation = BLT; break; // 0b 1011 <  lt
                case 0xC : operation = BGT; break; // 0b 1100 >  gt
                case 0xD : operation = BLE; break; // 0b 1101 <= le
                case 0xE : operation = B;   break; // 0b 1110 T  al
               
            } break;
           
        case MULTIPLY             : opcode = getVal(fetched->binaryInstruction, 21, 21);

            switch(opcode) {
           
                case 0: operation = MUL; break;
                case 1: operation = MLA; break;
               
            } break;
       
        case DATA_PROCESSING      : opcode = getVal(fetched->binaryInstruction, 24, 21);
       
            switch(opcode) {
           
                //Using hex values since binary isnt available.
               
                case 0x0 : operation = AND; break; // 0b 0000
                case 0x1 : operation = EOR; break; // 0b 0001
                case 0x2 : operation = SUB; break; // 0b 0010
                case 0x3 : operation = RSB; break; // 0b 0011
                case 0x4 : operation = ADD; break; // 0b 0100
                case 0x8 : operation = TST; break; // 0b 1000
                case 0x9 : operation = TEQ; break; // 0b 1001
                case 0xA : operation = CMP; break; // 0b 1010
                case 0xC : operation = ORR; break; // 0b 1100
                case 0xD : operation = MOV; break; // 0b 1101
               
            } break;
       
        case SINGLE_DATA_TRANSFER : opcode = getVal(fetched->binaryInstruction, 20, 20) ;

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

        case DATA_PROCESSING :      decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                    decoded->s        = getVal(fetched->binaryInstruction, 20, 20) ; 
                                    decoded->i        = getVal(fetched->binaryInstruction, 25, 25) ;
                                    decoded->rn       = getVal(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rd       = getVal(fetched->binaryInstruction, 15, 12) ;
                                    decoded->op2      = getVal(fetched->binaryInstruction, 11,  0) ;
                                    break;
        case MULTIPLY :             decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                    decoded->a        = getVal(fetched->binaryInstruction, 21, 21) ;
                                    decoded->s        = getVal(fetched->binaryInstruction, 20, 20) ;
                                    decoded->rd       = getVal(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rn       = getVal(fetched->binaryInstruction, 15, 12) ;
                                    decoded->rs       = getVal(fetched->binaryInstruction, 11,  8) ;
                                    decoded->rm       = getVal(fetched->binaryInstruction,  3,  0) ;
                                    break;
        case SINGLE_DATA_TRANSFER : decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                    decoded->i        = getVal(fetched->binaryInstruction, 25, 25) ;
                                    decoded->p        = getVal(fetched->binaryInstruction, 24, 24) ;
                                    decoded->rn       = getVal(fetched->binaryInstruction, 19, 16) ;
                                    decoded->rd       = getVal(fetched->binaryInstruction, 15, 12) ;
                                    decoded->u        = getVal(fetched->binaryInstruction, 23, 23) ;
                                    decoded->l        = getVal(fetched->binaryInstruction, 20, 20) ;
                                    decoded->offset   = getVal(fetched->binaryInstruction, 11,  0) ;
                                    break;
        case BRANCH               : decoded->l        = getVal(fetched->binaryInstruction, 31, 28) ;
                                    decoded->offset   = getVal(fetched->binaryInstruction, 23,  0) ;
                                    break;
        case SPECIAL              : break;

  }
 
  decoded->pending = T;
 
}

//Function to print the state of the memory and registers when the program finishes.
void printFinalState(void){
 printf("Registers:\n");
 for (int i = 0; i<17 ; i++) {
   if (i!=13 && i!=14) {
    
   printf("%-4s: %10i (0x%08x)\n", ARMReg[(i)].ident, ARMReg[i].reg, ARMReg[i].reg );

}
 }
 
 printf("Non-zero memory:\n");
 for (int count = 0; count < 16384; count+=4) {
  if (fetchFromMem(count)!=0) {
    printf("0x%08x: 0x%02x%02x%02x%02x\n", (count), memPtr[count], memPtr[count + 1],
    memPtr[count + 2], memPtr[count + 3]);
  }
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
     fseek( file, 0, SEEK_END );
     int instructionsSize = ftell(file);
     fseek( file, 0, SEEK_SET );

     //Load the binary data into the memory array.
     fread(memPtr, 1, instructionsSize, file );

     /* file closing */
     fclose(file);



     //Pipeline execution of instructions.
   
     //main pipeline loop.
     while ( T ) { //Loops infinitely until halt instruction is called



        if ( decoded->pending == T ) {

            if (decoded->operation == ANDEQ) { //check for special halt instruction
                break;
            }

            if ( checkCond() ) { //check if conditions match CPSR

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
