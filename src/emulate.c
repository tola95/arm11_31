
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

 //Since static const didn't work, this is an alternative 
//which is safer than just writing the number.
enum {REG = 17, MEM = 65536};

/*typedef only gives name to the struct ARM_STATE so that 
you don't have to right struct ARM_STATE when you initialise
the struct. This leads you to only have to write State when 
initialising*/

//creating struct for each register.
typedef struct ARM_REGISTER {
  char ident[4];
  uint32_t reg;
} Register;

//Creating a pointer to the ARM Machine main memory.
uint8_t *memPtr;

//This method is called by the main method to initialise all the memory.
void initMem() {
  uint8_t *memory = (uint8_t *) calloc(MEM, 1);
  memPtr = memory;
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
void and(uint32_t rn, uint32_t op2, uint32_t des) {
  ARMReg[des].reg = ARMReg[rn].reg & op2;
}

void eor(uint32_t rn, uint32_t op2, uint32_t des) {
  ARMReg[des].reg = ARMReg[rn].reg ^ op2;
}

void sub(uint32_t rn, uint32_t op2, uint32_t des) {
  uint32_t one = 1;
  ARMReg[des].reg = ARMReg[rn].reg + (~op2 + one);
}

void rsb(uint32_t rn, uint32_t op2, uint32_t des) {
  sub(op2, rn, des);
}

void add(uint32_t rn, uint32_t op2, uint32_t des) {
  ARMReg[des].reg = ARMReg[rn].reg + op2;
}

uint32_t tst(uint32_t rn, uint32_t op2) {
  uint32_t result = ARMReg[rn].reg & op2;
  return result;
}

uint32_t teq(uint32_t rn, uint32_t op2) {
  uint32_t result = ARMReg[rn].reg ^ op2;
  return result;
} 

uint32_t cmp(uint32_t rn, uint32_t op2) {
  uint32_t one = 1;
  uint32_t result = ARMReg[rn].reg + (~op2 + one);
  return result;
}

void orr(uint32_t rn, uint32_t op2, uint32_t des) {
  ARMReg[des].reg = ARMReg[rn].reg | op2;
}

void mov(uint32_t op2, uint32_t des) {
  ARMReg[des].reg = op2;
}

//This function chooses which the right operation depending on the opcode
void operateOp(uint32_t opcode, uint32_t rn, uint32_t op2, uint32_t des) {
  switch(opcode) {
    case 0x00000000 : and(rn, op2, des);
    break;
    case 0x00000001 : eor(rn, op2, des);
    break;
    case 0x00000002 : sub(rn, op2, des);
    break;
    case 0x00000003 : rsb(rn, op2, des);
    break;
    case 0x00000004 : add(rn, op2, des);
    break;
    case 0x00000008 : tst(rn, op2);
    break;
    case 0x00000009 : teq(rn, op2);
    break;
    case 0x0000000a : cmp(rn, op2);
    break;
    case 0x0000000c : orr(rn, op2, des);
    break;
    case 0x0000000d : mov(op2, des);
  }
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
        int pending;
    } ;

//Struct representing fetched instruction.
    struct fetchedInstruction {
        uint32_t binaryInstruction;
        int pending;
    } ;

 

//function to increment PC to next instruction address.
void incrementPC(){

    ARMReg[15].reg += 1;//PC is a macro defined earlier

}


//function to fetch the instruction at the current instruction address stored in PC.
void fetchNextInstruction(struct fetchedInstruction fetched){

    fetched.binaryInstruction = ARMReg[15].reg; //PC is a macro defined earlier.
    fetched.pending = 1;
    
}
 
//function to execute decoded instructions then clear decoded instructions pending flag.
void execute(struct decodedInstruction decoded){

    enum mnemonic op = decoded.operation;
    switch (op){
        case AND : and(decoded.rn, decoded.op2, decoded.rd);
        break;
        case EOR : eor(decoded.rn, decoded.op2, decoded.rd);
        break;
        case SUB : sub(decoded.rn, decoded.op2, decoded.rd);
        break;
        case RSB : rsb(decoded.rn, decoded.op2, decoded.rd);
        break;
        case ADD : add(decoded.rn, decoded.op2, decoded.rd);
        break;
        case TST : tst(decoded.rn, decoded.op2);
        break;
        case TEQ : teq(decoded.rn, decoded.op2);
        break;
        case CMP : cmp(decoded.rn, decoded.op2);
        break;
        case ORR : orr(decoded.rn, decoded.op2, decoded.rd);
        break;
        case MOV : mov(decoded.op2, decoded.rd);
        default:
        break;
  }
    
}



//function to decoded a previously fetched instrcution.
void decode(struct fetchedInstruction fetched){
}



 //Main Function
 int main(int argc, char **argv) {

   initMem();

	 /* file loading */
	 FILE *file = fopen(argv[1], "rb");

	 if (file == NULL){
		 perror("Error: Could not open file.");
		 exit(EXIT_FAILURE);
	 }
     
     
     /* Calculate file size then from the size, the number of 32-bit instructions in the file */
     const int bytesPerInstruction = 4;
     fseek( file, 0, SEEK_END );
     int instructionsSize = ftell(file)/bytesPerInstruction;
     fseek( file, 0, SEEK_SET );

     //Load the binary data into the memory array.
	 fread(memPtr, bytesPerInstruction, instructionsSize, file );

	 /* file closing */
	 fclose(file);



    //Pipeline execution of instructions.
    //Start with no instructions decoded and no instructions fetched.
    struct fetchedInstruction fetched;
    struct decodedInstruction decoded;
    fetched.pending = 0;
    decoded.pending = 0;    

    
    //main pipeline loop.
     while ( 1 ) {



        if ( decoded.pending == 1 ) {

            execute(decoded);

        }

        if ( fetched.pending == 1 ) {

            decode(fetched);
            
        }

        if (ARMReg[15].reg <  MEM) {

            fetchNextInstruction(fetched);
        }


        incrementPC();

     
     }
     

	 return EXIT_SUCCESS;

 }


