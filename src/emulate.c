
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

 

 /*int binaryToDec(int bin[4]) {
 	//Doesn't work correctly yet
 	int answer = 0;
 	int power = 3;
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
 	
 	
 }

 int *fetchfromreg(int rn[]) {

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

 }*/

 

 

 

 

 //Main Function
 int main(int argc, char **argv) {


	 /* initialise memory to 0 */
	 int size = 16384;
	 uint32_t zero = 0;
	 uint32_t *Memory = malloc( size * sizeof(uint32_t) );

	 int i;
	 for (i = 0; i < 16384; i++ ){
		 Memory[i] = zero;
	 }

	 /* file loading */
	 FILE *file = fopen(argv[1], "rb");

	 if (file == NULL){
		 perror("Error: Could not open file.");
		 exit(EXIT_FAILURE);
	 }
     
     
     /* Calculate file size then from the size, the number of 32-bit instructions */  
     fseek( file, 0, SEEK_END );
     int instructionsSize = ftell(file)/4;
     fseek( file, 0, SEEK_SET );
	
	 printf("%d \n", instructionsSize);

	 

	 fread(Memory, 4, instructionsSize, file );

	 /* file closing */
	 fclose(file);

	 i = 0;
	 for ( i = 0; i < 16384; i++ ) {

	    printf("%x" , Memory[i] );

	 }




	 return EXIT_SUCCESS;

 }

