
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>

 int and(int rn, int op2) {
	 // ToDo : Function not yet implemented...
	 return 0;
 }

 int eor(int rn, int op2) {
	 // ToDo : Function not yet implemented...
	 return 0;
 }

 int sub(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return rn - op2 ;
  }

 int rsb(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return op2 - rn ;
  }

 int add(int rn, int op2) {
 	 return rn + op2 ;
  }

 void tst(int rn, int op2) {
	 // As AND but result not written
	 // ToDo : Function not yet implemented...
 	 ;
  }

 void teq(int rn, int op2) {
	 // As OR but result not written
	 // ToDo : Function not yet implemented...
 	 ;
  }

 void cmp(int rn, int op2) {
	 // As SUB but result not written
	 // ToDo : Function not yet implemented...
 	 ;
  }

 int orr(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return 0;
  }

 int mov(int rn, int op2) {
	 // ToDo : Function not yet implemented...
	 return op2;
 }

 //Main Function
 int main(int argc, char **filePath[]) {
	 FILE *file = fopen(filePath[0],"r");

	 if (file == 0){
		 printf("Error: Could not open file.\n");
	 }

	 int x = 0;
	 int mem[8][65536];

	 while(1){
		 //fread

	 }

	 return EXIT_SUCCESS;

 }

