
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

const int NUMBER_OF_REGISTERS = 17; // Number of registers
const int memSize = 16384; //maximum number of instructions that can be stored.
uint32_t zero = 0;


//Struct representing the state of machine
  struct arm_State {
 	uint32_t reg[17];
	uint32_t memory[16384];	
 }  ;

//Helper method to write to register
 void putinreg(struct arm_State state, uint32_t rd, uint32_t op2) {
 	state.reg[rd] = op2;	
 }

//Helper method to fetch from register
 uint32_t fetchfromreg(struct arm_State state, uint32_t rn) {
	return state.reg[rn];
 }

//Helper method for selecting bits
 uint32_t selectbits(uint32_t i, int first, int last) {
	uint32_t mask = 0;
	int count = first;

        //Create the 32 bit mask
	while (count>=last) {
		mask = mask + 1  * pow(2, count);
		count--;
	}
        
	//& operation to extract the required bits
	i &= mask;

	//Extract the bits by shift right by 'last' amount of bits
	i >>= last;

        return i;
 }

 

 //Data Processing Instructions

void and(struct arm_State state, uint32_t rn, uint32_t op2, int r) {
 	state.reg[r] = rn & op2;
 }

 void eor(struct arm_State state, uint32_t rn, uint32_t op2, int r) {
 	state.reg[r] = rn ^ op2;
 }

 void sub(struct arm_State state, uint32_t rn, uint32_t op2, int r) {
 	uint32_t one = 1;
 	uint32_t result = ~op2 + one;
 	state.reg[r] = result;
 }

/*
 void teq(struct arm_State state, uint32_t rn, uint32_t op2) {
	 // As EOR but result not written
 	uint32_t op1 = fetchfromreg(state, rn);
 	uint32_t result = op1 ^ op2; // 4, 5
  }
*/
 void cmp(struct arm_State state, uint32_t rn,  uint32_t op2) {
	 // As SUB but result not written
 	 uint32_t op1 = fetchfromreg(state, rn);
 	 uint32_t result = op1 - op2;
	 uint32_t newbit = 0;
         uint32_t CPSR = fetchfromreg(state, 15); //Fetch contents of reg 15 (CPSR)
	 
	 if (result<0 ) {
	 	newbit = selectbits(CPSR, 31, 31) | 1; //Change first bit to 1
	 } else {
		newbit = selectbits(CPSR, 31, 31) & 0; //Change first bit to 0
	}

	putinreg(state, 15, newbit * pow(2, 31));



  }

 void orr(struct arm_State state, uint32_t rn, uint32_t op2, uint32_t rd) {
 	uint32_t op1 = fetchfromreg(state, rn);
 	uint32_t result = op1||op2; 
 	putinreg(state, rd, result);

  }

 void mov(struct arm_State state, uint32_t op2, uint32_t rd) {
 	 // Move op2 to destination register
 	putinreg(state, rd, op2);

 }

 //Multiply Instructions

void multiply(struct arm_State state, uint32_t rs, uint32_t rm, uint32_t rd) {
	uint32_t arg1 = fetchfromreg(state, rm);
	uint32_t arg2 = fetchfromreg(state, rs);
	uint32_t result = arg1 * arg2 ; 
	putinreg(state, rd, result);
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


	 fread(ARM_State.memory, bytesPerInstruction, instructionsSize, file );

	 /* file closing */
	 fclose(file);


	// just to make sure its working.
	 printf("%d \n", instructionsSize);

	 i = 0;
	 for ( i = 0; i < memSize; i++ ) {

	    printf("%x " , ARM_State.memory[i] );

	 }

    printf("\n");
	 i = 0;
	 for ( i = 0; i < NUMBER_OF_REGISTERS; i++ ) {

	    printf("%x " , ARM_State.reg[i] );

	 }

    printf("\n");

        


	 return EXIT_SUCCESS;

 }

