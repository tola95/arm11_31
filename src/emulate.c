
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

//This macro makes it easier to reference the PC, leading to prettier code.
#define PC state.reg[15]

const int NUMBER_OF_REGISTERS = 17; // Number of registers
const int memSize = 16384; //maximum number of instructions that can be stored.
uint32_t zero = 0;

//Struct representing the state of machine
  struct arm_State {
 	uint32_t reg[17];
	uint32_t memory[16384];	
 }  ;

 

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
        uint32_t Rd;
        uint32_t Rn;
        uint32_t Rm;
        uint32_t Op2;
        int pending;
    } ;

//Struct representing fetched instruction.
    struct fetchedInstruction {
        uint32_t binaryInstruction;
        int pending;
    } ;


 
//function to increment PC to next instruction address.
void incrementPC( struct arm_State state ){

    PC += 1;//PC is a macro defined earlier

}

//function to fetch the instruction at the current instruction address stored in PC.
void fetchNextInstruction(struct arm_State state, struct fetchedInstruction fetched){

    fetched.binaryInstruction = state.memory[ PC ]; //PC is a macro defined earlier.
    fetched.pending = 1;
    
}
 
//function to execute decoded instructions then clear decoded instructions pending flag.
void execute(struct arm_State state, struct decodedInstruction decoded){

    enum mnemonic op = decoded.operation;
    switch (op){
    case AND: and(state, decoded.Rn, decoded.Op2, decoded.Rd); break;
    default: break;
    }

}

//function to decoded a previously fetched instrcution.
void decode(struct fetchedInstruction fetched){
}
 

 

 //Main Function
 int main(int argc, char **argv) {

	//arm state initialised.
	struct arm_State ARM_State;

	int i;
	//Initialise the registers to zero
	for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
		ARM_State.reg[i] = zero;
	}


	 /* initialise memory to 0 */
	 uint32_t zero = 0;

	 for (i = 0; i < memSize; i++ ){
		 ARM_State.memory[i] = zero;
	 }

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
	 fread(ARM_State.memory, bytesPerInstruction, instructionsSize, file );

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

            execute(ARM_State, decoded);

        }

        if ( fetched.pending == 1 ) {

            decode(fetched);
            
        }

        if ( PC <  memSize) {

            fetchNextInstruction(ARM_State, fetched);
        }


        incrementPC(ARM_State);




     
     }
     

    


    
	 return EXIT_SUCCESS;

 }



























