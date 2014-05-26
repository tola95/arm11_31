
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

  typedef struct {
 	uint32_t r[17];
 	
 } Reg ;

 

 int binaryToDec(int bin[4]) {
 	//Doesn't work correctly yet
 	int answer = 0;
 	int power = size - 1;
 	for (int i = 0; i<4; i++ ) {
 		answer = answer + (bin[i] * pow(2, power )) ;
 		power--;
 	}
 	return answer;
 }

 int *reverse(int arg[]) {

 }


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


 void putinreg(int rd[], int *arg) {
 	for (int i=0; i<32; i++) {
 		reg.r[binaryToDec(rd)][i] = arg
 	}
 	
 }

 int *fetchfromreg(int rn[]) {
 	int arg = binaryToDec(rn);
 	uint32_t *p;
 	p = Reg.r[arg];
 	return p;
 }

 

 void teq(int rn[4], int op2[12]) {
	 // As EOR but result not written
 	// Assuming EOR just works on the last bit
 	int *op1 = fetchfromreg(rn);
 	int result = op1[32 - 1] ^ op2[17 - 1]; // 4, 5
  }

 void cmp(int rn[], int op2[]) {
	 // As SUB but result not written
 	 int arg1 = binaryToDec(fetchfromreg(rn));
 	 int arg2 = binaryToDec(op2);
 	 int *result = decToBinary(arg1 - arg2);

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
			reg[i][j] = 0;
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

