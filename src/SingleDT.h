#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "optionals.h"

//  Used to store a machine code value in the specified memory address.
 void putInMem(uint32_t memAddr, uint32_t value) {

        uint8_t val = 0;
        uint32_t mask = 0xff000000;
        
        for (int i=3; i>=0; i--) {           //  Splits the 32bit value into 4 values of size 1 byte each.
            val = (value & mask) >> (i*8) ;  //  Then stores these bytes in 4 consecutive byte addresses.
            memPtr[memAddr + i] = val;
            mask >>= 8;
        }    
}



/******************************************************************************************************/
/******************Helpers to set bits for the different cases as explained in spec Pg 15**************/
/******************************************************************************************************/

uint32_t caseNumericConstant( uint32_t rd, uint32_t address ) {
  //Numeric constant case
  uint32_t offset = 0;
  uint32_t result = 0x0; 
  
  result |= (1 << 20) ;    //set L bit because only ldr instructions get to this case
  result |= (01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (0 << 25);     //i is not set since numeric constant
  result |= (1 << 23);   //u bit is set
  

  
    
    //Put value of expression in four bytes at the end of the assembled program : ToDo
    putInMem((numberOfLines)*4, address); //newaddress is the next available adress at the end of the assembled program
   
    //Use this address, with PC as base reg and calculated offset, in assembled ldr instrction
    result |= (15 << 16); //with PC as base reg (rn)
    offset = (((numberOfLines + 1) - lineNumber) * 4) - 8;  //Compute the offset between current location and newly created one
                                           //Current adress is the address of this current instruction we're decoding
    result |= offset;
    result |= (1 << 24); //Always pre index 
    result |= (14 << 28); //Cond bits always 'al'

     numberOfLines++;
    return result;

  
    
    
  
}

uint32_t casePreIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression, enum mnemonic opcode) {
  //Pre indexed address specification
  uint32_t result = 0x0;
 
   switch (opcode) {
    
    case LDR : result |= (1 << 20) ;    //set L bit
    case STR : result |= (0 << 20) ;    // don't set L bit
    default : break;         
  }

  result |= (0x01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (0x00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (address << 16);    //with rn as base reg 
  result |= (1 << 24); //pre index
  result |= (0 << 25); //i is set since address is register
  result |= expression;
  result |= (14 << 28); //Cond bits always 'al'
  result |= (1 << 23);
  if (minus == 1) {
    result &= 0xff7fffff;
  }
  if (isRegister == 1) {
    result |= 0x02000000;
  }
  return result;
}

uint32_t casePostIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression, enum mnemonic opcode) {
  //Post indexed address specification
  uint32_t result = 0x0;
   
   switch (opcode) {
    
    case LDR : result |= (1 << 20) ;    //set L bit
    case STR : result |= (0 << 20) ;    // don't set L bit
    default : break;         
  }
  result |= (0x01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (0x00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (address << 16);    //with rn as base reg
  result |= (0 << 24); //post index
  result |= (0 << 25); 
  result |= expression;
  result |= (14 << 28); //Cond bits always 'al'
  result |= (1 << 23);   //u bit is set
  if (minus == 1) {
    result &= 0xff7fffff;
  }
  if (isRegister == 1) {
    result |= 0x02000000;
  }

  return result;
}



/**************************************************************/
/*******************LDR and STR Functions**********************/
/**************************************************************/

uint32_t ldr(char* rd,  char* address, enum mnemonic opcode) { 
  char *ptr = &rd[1];
  char *ad = &address[3];
  switch(address[0]) {
    // Case where address is =0x....
    case '='  :  if ( strtol(ad, NULL, 16) > 255 ) {
                   return caseNumericConstant(atoi(ptr), strtol(ad, NULL, 16));
                 } else {
                   return convertMov(MOV, rd, address);
                 }
    //Case where address is [rn] , [rn, #expression] or [rn,{+/-}rm{,<shift>}]
    case '['  :  

      if (strstr(address, "],")==NULL) { //String "]," never occurs in pre index but always occurs in post index

        switch (noOfArgs(address)) {

           case 1 : return casePreIndexedAdSpec(atoi(ptr), getRn1(address), 0, opcode); //<ldr/str> rn [rm]

           case 2 :  return casePreIndexedAdSpec(atoi(ptr), getRn(address), getExpr(address), opcode); //<ldr/str> rn [rm, #0x..]

           case 3 : 

           if (strstr(address, "+")!=NULL) {
             minus = 0;
           }
            if (strstr(address, "-")!=NULL) {
             minus = 1;
           }
           pre = 1; 
            if (strstr(address, "lsl")!=NULL) {
             shift = 0;
           } else if (strstr(address, "lsr")!=NULL) {
             shift = 1;
           } else if (strstr(address, "asr")!=NULL) {
             shift = 2;
           } else if (strstr(address, "ror")!=NULL) {
             shift = 3;
           } else {
             ;
           }
           
           return caseOptional(atoi(ptr), address, opcode); 

           default : return 0; //Should never reach this
        }

      } else {
        
         switch (noOfArgs(address)) {

           case 2 :  
          
           return casePostIndexedAdSpec(atoi(ptr), getRn1(address), getExpr1(address), opcode);//<ldr/str> rn [rm], #0x..

           case 3 :  
           
           if (strstr(address, "+")!=NULL) {
             minus = 0;
           }
            if (strstr(address, "-")!=NULL) {
             minus = 1;
           }
           
            if (strstr(address, "lsl")!=NULL) {
             shift = 0;
           } else if (strstr(address, "lsr")!=NULL) {
             shift = 1;
           } else if (strstr(address, "asr")!=NULL) {
             shift = 2;
           } else if (strstr(address, "ror")!=NULL) {
             shift = 3;
           } else {
             ;
           }
           
           return caseOptional(atoi(ptr), address, opcode); 

           default : return 0; //Should never reach this

         }
      }
      
    
    default :  return 0;  
    
  } 
}

uint32_t str(char* rd,  char* address, enum mnemonic opcode) {
   char *ptr = &rd[1];
  
   if (strstr(address, "],")==NULL) { //String "]," never occurs in pre index but always occurs in post index

         switch (noOfArgs(address)) {

           case 1 : return casePreIndexedAdSpec(atoi(ptr), getRn1(address), 0, opcode);

           case 2 : if (strstr(address, "]\0")!=NULL) {
                    return casePreIndexedAdSpec(atoi(ptr), getRn(address), getExpr(address), opcode);
                    } else {
                    return casePreIndexedAdSpec(atoi(ptr), getRn(address), getExpr1(address), opcode);
                    }
           case 3 : 
           if (strstr(address, "+")!=NULL) {
             minus = 0;
           }
            if (strstr(address, "-")!=NULL) {
             minus = 1;
           }
           pre = 1; 
            if (strstr(address, "lsl")!=NULL) {
             shift = 0;
           } else if (strstr(address, "lsr")!=NULL) {
             shift = 1;
           } else if (strstr(address, "asr")!=NULL) {
             shift = 2;
           } else if (strstr(address, "ror")!=NULL) {
             shift = 3;
           } else {
             ;
           }
           
           return caseOptional(atoi(ptr), address, opcode); 

           default : return 0; //Should never reach this

         }

   } else {

         switch (noOfArgs(address)) {

           case 2 :
                    return casePostIndexedAdSpec(atoi(ptr), getRn1(address), getExpr1(address), opcode);
                    
           case 3 : 
           if (strstr(address, "+")!=NULL) {
             minus = 0;
           }
            if (strstr(address, "-")!=NULL) {
             minus = 1;
           }
            
            if (strstr(address, "lsl")!=NULL) {
             shift = 0;
           } else if (strstr(address, "lsr")!=NULL) {
             shift = 1;
           } else if (strstr(address, "asr")!=NULL) {
             shift = 2;
           } else if (strstr(address, "ror")!=NULL) {
             shift = 3;
           } else {
             ;
           }
           
           return caseOptional(atoi(ptr), address, opcode); 

           default : return 0; //Should never reach this

         }
          
   }
 
}

/*
int main (int argc, char** argv) {
 
  FILE * filehandle;

  //open file
    filehandle = fopen(argv[1],"r");

  
  char *token ;

  fread(op, sizeof(char), 20 , filehandle);
  //Tokeniser
  token = strtok(op, " ");
    ops = token;
    
   printf("%s\n", token);

   token = strtok(NULL, ",");

  
  
    fclose(filehandle);
  
    uint32_t example = ldr("r2", "=0x20200020");
    //uint32_t ex2 = getExpr("[r19, #0xfff]");
    printf("0x%08x\n", example);
    printf("works\n");
    return 0;
}
*/
