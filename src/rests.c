/*
 * tests.c
 *
 *  Created on: 27 May 2014
 *      Author: na4113
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>



//Struct representing the state of machine
  struct arm_State {
 	uint32_t reg[17];
	uint32_t memory[16384];
 }  ;

  int *decToBinary(int i) {
 	 int answer[32]; // 1
 	 int *p;
 	 int j = 0;
 	 while (i>0) {
         answer[j] = i % 2 ;
         i = floor(i/2);
         j++;
 	 }
 	 p = answer;
 	 return p;
  }

void putinreg(struct arm_State state, uint32_t rd, uint32_t op2) {
	state.reg[rd] = op2;
}

uint32_t fetchfromreg(struct arm_State state, uint32_t rn) {
	return state.reg[rn];
}

uint32_t rsb(struct arm_State state, uint32_t rn, uint32_t op2) {
	// As for SUB, but the operation is commuted.
	uint32_t op1 = fetchfromreg(state, rn);
	uint32_t result = op2 - op1;
	return result;
}

uint32_t add(struct arm_State state, uint32_t rn, uint32_t op2) {
	// Adds a specified 32-bit binary number number to the register RN
	uint32_t op1 = fetchfromreg(state, rn);
	uint32_t result = op2 + op1;
	return result;
}

void tst(struct arm_State state, uint32_t rn, uint32_t op2) {
	// As AND but result not written
	uint32_t op1 = fetchfromreg(state, rn);
	uint32_t result = op1 & op2;
}

// Helper method for selecting bits
 uint32_t selectbits(uint32_t i, int first, int last) {
	uint32_t mask = 0;
	int count = first;

        // Create the 32 bit mask
	while (count>=last) {
		mask = mask + 1  * pow(2, count);
		count--;
	}

	// & operation to extract the required bits
	i &= mask;

	//Extract the bits by shift right by 'last' amount of bits
	i >>= last;

        return i;
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

 // Multiply Instructions
void multiply(struct arm_State state, uint32_t rs, uint32_t rm, uint32_t rd) {
	uint32_t arg1 = fetchfromreg(state, rm);
	uint32_t arg2 = fetchfromreg(state, rs);
	uint32_t result = arg1 * arg2 ;
	putinreg(state, rd, result);
}



int main(void) {

	//arm state initialized.
	struct arm_State ARM_State;

	int i;

	//Initialize the registers to 0
	for (i = 0; i < 17; i++) {
		ARM_State.reg[i] = 0;
	}

	/* initialize memory to 0 */
	for (i = 0; i < 16384; i++ ){
		ARM_State.memory[i] = 0;
	}

	ARM_State.reg[5] = 53;
	uint32_t roar = add(ARM_State, 5, 24);
	printf("%b\n", roar);


	return 0;

}
