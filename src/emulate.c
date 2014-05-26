
 /*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>

struct Machine {
	int reg[17];
	int mem[16000];
};

struct Machine arm11;

void initializo(void) {
	for (int i = 0; i < 16000; ++i) {
		arm11.mem[i] = 0;
	}
	for (int i = 0; i < 17; ++i) {
		arm11.reg[i] = 0;
	}
}

int dectobinary(int n) {  /* Method to convert decimal to binary.*/
    int r = 0;
	int i = 1;
	int result = 0;
    while (n!=0) {
        r = n % 2;
        n /= 2;
        result += r*i;
        i *= 10;
    }
    return result;
}

int bintodecimal(int n) {  /* Method to convert binary to decimal.*/
    int result = 0;
	int i = 1;
	int r = 0;
    while (n!=0) {
        r = n % 10;
        n /= 10;
        result += r*i;
        i *= 2;
    }
    return result;
}

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
	rn = bintodecimal(rn);
	op2 = bintodecimal(op2);
	int rd = dectobinary(op2 - rn);
 	 return rd;
  }

 int add(int rn, int op2) {
	arm11.reg[rn] = bintodecimal(arm11.reg[rn]);
	op2 = bintodecimal(op2);
	int rd = dectobinary(op2 + rn);
 	 return rd;
  }

 void tst(int rn, int op2) {
	 rn = bintodecimal(rn);
	op2 = bintodecimal(op2);
	int rd = dectobinary(op2 & rn);
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
