/*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

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

//Opcode and operation for when operand2 is not an immediate constant.
uint32_t lsl(uint32_t rm, uint32_t shift) {
  return rm << shift;
}

uint32_t lsr(uint32_t rm, uint32_t shift) {
  return rm >> shift;
}

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



 

//function to increment PC to next instruction address.
void incrementPC(void){

    PC_ += 1;//PC_ is a macro defined earlier

}


//function to fetch the instruction at the current instruction address stored in PC.
void fetchNextInstruction(){

    fetched->binaryInstruction = PC_; //PC_ is a macro defined earlier.
    fetched->pending = T;
    
}
 
//function to execute decoded instructions and clear decoded instructions pending flag.
void executeDecodedInstruction(){


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
void decodeFetchedInstruction(){
 
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
