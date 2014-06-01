#include <stdio.h>
#include <stdlib.h>

int add(int rn, int op2) {
 	 return rn + op2 ;
  }

int sub(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return rn - op2 ;
  }

 int rsb(int rn, int op2) {
	 // ToDo : Function not yet implemented...
 	 return op2 - rn ;
  }

int and(int rn, int op2) {
	 // ToDo : Function not yet implemented...
	 return 0;
 }

 int eor(int rn, int op2) {
	 // ToDo : Function not yet implemented...
	 return 0;
 }

 int orr(int rn, int op2) {
 	 // ToDo : Function not yet implemented...
  	 return 0;
   }

  int mov(int rn, int op2) {
 	 // ToDo : Function not yet implemented...
 	 return op2;
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

 //int mul()



//int main(int argc, char **argv) {
//  return EXIT_SUCCESS;
//}

 int main(void) {
	 const char (*fname)[];
	 if (fname[sizeof(fname)-2] == "." && fname[sizeof(fname)-1] == "s") {
		 FILE *stream = fopen(fname, "r");
		 printf("works\n");
	 } else {
		 fprintf(stderr, "Must be a source file in assembly language!");
		 exit(EXIT_FAILURE);
	 }
 }
