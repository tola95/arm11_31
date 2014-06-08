#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "HelpersDT.h"


typedef struct getArgs {
  char *rm;
  char *rn;
  char *shift;
} Args;


uint32_t caseOptional(uint32_t rd, char* address, enum mnemonic opcode) {
  uint32_t rn = 0;
  uint32_t rm = 0;
  uint32_t expression = 0;
  uint32_t result = 0x0;

  Args *args = malloc(sizeof(Args));
  
  args->rn = strtok(address, ","); 
 
  args->rm = strtok(NULL, ",");
 
  args->shift = strtok(NULL, "");

  if (strstr(address, "],") == NULL) {
    const char *p1 = strstr(args->rn, "[r")+2;
    rn = atoi(p1);
  } else {
    rn = getRn1(args->rn);
  }

  if (strstr(address, "-") == NULL) {
    minus = 0;
    const char *p1 = strstr(args->rm, "r")+1;
    rm = atoi(p1);
  } else {
    minus = 1;
    const char *p1 = strstr(args->rm, "-r")+2;
    rm = atoi(p1);
  }

  if (strstr(args->shift, "#")) {
  const char *p2 = strstr(args->shift, "#")+1;
  expression = atoi(p2);
  } else {
  const char *p2 = strstr(args->shift, ", ")+7;
  expression = atoi(p2);
  isRegister = 1;
  }

  switch (opcode) {
    
    case LDR : result |= (1 << 20) ;    //set L bit
    case STR : result |= (0 << 20) ;    // don't set L bit
    default : break;         
  }

  result |= (0x01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (0x00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (rn << 16);    //with rn as base reg
  if (pre == 1) {
    result |= (1 << 24);
  } else {
    result |= (0 << 24);
  }
   result |= (1 << 25); //i is set since address is register
   result |= (14 << 28); //Cond bits always 'al'
   if (minus == 1) {
    result |= (0 << 23);
  } else {
    result |= (1 << 23);
  }
  
  result |= rm;
  result |= (shift << 5);

  if (isRegister == 0) {
  result |= (expression << 7);
  } else {
  result |= (expression << 8);
  }

  if (isRegister == 1) {
    result |= (0 << 4);
  } 
  
  return result;


}

//int main(void) {
//  char string[] = "[r1,r2,lsr #2]";
 // printf("%x\n", caseOptional(0, string ));
//  return 0; 
//}
