
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

  typedef struct {
 	uint32_t r[17];
 	
  } Reg ;

 

 int dectobinary(int n) {  /* Method to convert decimal to binary.*/
     int result = 0,
    		 i = 1,
    		 r = 0;
     while (n != 0) {
         r = n % 2;
         n /= 2;
         result += r*i;
         i *= 10;
     }
     return result;
 }

 int bintodecimal(int n) {  /* Method to convert binary to decimal.*/
     int result = 0,
    		 i = 1,
    		 r = 0;
     while (n != 0) {
         r = n % 10;
         n /= 10;
         result += r*i;
         i *= 2;
     }
     return result;
 }

 int *reverse(int arg) {

 }

 int sub(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return rn - op2 ;
  }

 int rsb(int rn, int op2) {

 }

 int add(int rn, int op2) {
	Reg[rn] = bintodecimal(Reg[rn]);
	op2 = bintodecimal(op2);
	int rd = dectobinary(op2 + rn);
 	 return rd;
  }

 void tst(int rn, int op2) {
	 // As AND, but not written
	 uint32_t op1 = fetchfromreg(Reg[rn]);
	 uint32_t result = op1 ^ op2;
 }

 void putinreg(int rd, int *arg) {
 	for (int i=0; i<32; i++) {
 		Reg[bintodecimal(rd)][i] = arg;
 	}
 	
 }

 uint32_t fetchfromreg(int rn) {
 	int arg = bintodecimal(Reg[rn]);
 	uint32_t *p = Reg[arg];
 	return p;
 }

 

 void teq(int rn[4], int op2[12]) {
	 // As EOR but result not written
 	// Assuming EOR just works on the last bit
 	uint32_t op1 = fetchfromreg(rn);
 	uint32_t result = op1[32 - 1] ^ op2[17 - 1]; // 4, 5
  }

 void cmp(int rn, int op2[]) {
	 // As SUB but result not written
 	 int arg1 = bintodecimal(fetchfromreg(Reg[rn]));
 	 int arg2 = bintodecimal(op2);
 	 int *result = dectobinary(arg1 - arg2);
  }

 void orr(int rn[], int op2[], int rd[]) {
 	// Assuming OR just works on the last bit.
 	int *op1 = fetchfromreg(rn);
 	int result = op1[32 - 1]||op2[17 - 1]; // 6, 7
 	int answer[32]; // 8
 	answer[32 - 1] = result; // 9
 	for (int i = 0; i<32-1; i++) { //10
 		answer[i] = 0;
 	}
 	putinreg(rd, answer);

  }

 void mov(int op2[], int rd[]) {
 	 // Move op2 to destination register
 	putinreg(rd, op2);

 }

 

 

 

 

 //Main Function
 int main(void) {
/*
	 We have to do the memory (array of arrays (16384 X 32 bits))
	 We have to do the registers (array of arrays 17 X 32 bits)
*/
	 for(int i=0; i<17; i++) { // 11
		for (int j=0; j<32; j++) { //12
			Reg[i][j] = 0;
		}
	 }
     
/*
	 Check cond (first four bits) and S fields and set CPSR flag accordingly
	 Check Immediate Operand and update Operand 2 acordingly.
	 Check Opcode and 
*/
	 printf("works\n");
	 return EXIT_SUCCESS;

 }

