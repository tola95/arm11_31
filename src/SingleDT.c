#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Temporary stuff to ensure compilation

char inst[] = "ldr";// Main should pass me inst(the instruction) <ldr/str>
uint32_t memPtr[16];// Memory or wherever we'll be storing the assembled code
int newaddress = 15; // Next available address after the aeembled program
int currentaddress = 10; //Current location of the assembled program

//Single Data Transfer Functions

 uint32_t getRm1(char *address) {
   
   const char *p1 = strstr(address, "[r")+2;
   const char *p2 = strstr(p1, "]");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return atoi(res);
 }

uint32_t getRm(char *address) {

   const char *p1 = strstr(address, "[r")+2; 
   const char *p2 = strstr(p1, ",");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return atoi(res);
 }

 uint32_t getExpr(char *address) {
  
   const char *p1 = strstr(address, "#")+1;
   const char *p2 = strstr(p1, "]");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return strtol(res, NULL, 16);
 }

 int noOfArgs(char *address) {
   if(strstr(address, ",") != NULL) {
     return 2;
   } else {
     return 1;
   }
 }

uint32_t caseNumericConstant( uint32_t rd, uint32_t address ) {

  uint32_t offset = 0;
  uint32_t result = 0x0; 
  
  result |= (1 << 20) ;    //set L bit
  
  result |= (01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  
  result |= (00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  
  result |= (0 << 25);     //i is not set since numeric constant
  

  if (address > 255) {
    
    //Put value of expression in four bytes at the end of the assembled program : ToDo
    memPtr[newaddress] = address; //newaddress is the next available adress at the end of the assembled program
    
    //Use this address, with PC as base reg and calculated offset, in assembled ldr instrction
    result |= (15 << 16); //with PC as base reg (rn)

    offset = newaddress - currentaddress;  //Compute the offse between current location and newly created one
                                           //Current adress is the address of this current instruction we're decoding
    result |= offset;

    result |= (1 << 24); //Always pre index 
    
    return result;

  } else {
    
    return 0;//mov(rd, address);
  }
}

uint32_t casePreIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression) {
  
  uint32_t result = 0x0;
 
  if ( strcmp( inst, "ldr") == 0 ) {
    result |= (1 << 20) ;    //set L bit
  } else {
    result |= (0 << 20) ;    // don't set L bit
  }
  
  result |= (0x01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (0x00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (address << 16);    //with PC as base reg (rn)
  result |= (1 << 24); //pre index
  result |= (0 << 25); //i is set since address is register

     result |= expression;
  return result;
}

uint32_t casePostIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression) {
  
  uint32_t result = 0x0;
 
  if ( strcmp( inst, "ldr") == 0 ) {
    result |= (1 << 20) ;    //set L bit
  } else {
    result |= (0 << 20) ;    // don't set L bit
  }
  
  result |= (0x01 << 26) ; //bits 27 and 26 are 0 and 1 respectively
  result |= (0x00 << 21) ; //bits 22 and 21 are 0 and 0 respectively
  result |= (rd << 12);    //bits 15 t0 12 represent the rd
  result |= (address << 16);    //with PC as base reg (rn)
  result |= (0 << 24); //pre index
  result |= (0 << 25); //i is set since address is register

     result |= expression;
  return result;
}


uint32_t ldr(char* rd,  char* address) {
  char *ptr = &rd[1];
  char *ad = &address[3];
  switch(address[0]) {
  
    case '='  :  return caseNumericConstant(atoi(ptr), strtol(ad, NULL, 16));
    
    case '['  :  

      if (strstr(address, "],")==NULL) {

        if (noOfArgs(address) == 1) {
          return casePreIndexedAdSpec(atoi(ptr), getRm1(address), 0);
        } else if (noOfArgs(address) == 2) {
          return casePreIndexedAdSpec(atoi(ptr), getRm(address), getExpr(address));
        } else {
          return 0; //Optional. Do later
        }

      } else {
        
        if (noOfArgs(address)== 2)  {
          return casePostIndexedAdSpec(atoi(ptr), getRm1(address), getExpr(address));
        }
      }
      
    
    default :  return 0;  
    
  } 
}

uint32_t str(char* rd,  char* address) {
   char *ptr = &rd[0];
  switch (address[0]) {

    case '[' : 

       if (strstr(address, "],")==NULL) {

         if (noOfArgs(address) == 1) {
           return casePreIndexedAdSpec(atoi(ptr), getRm1(address), 0);
         } else if (noOfArgs(address) == 2) {
       
           return casePreIndexedAdSpec(atoi(ptr), getRm(address), getExpr(address));
         } else {
           return 0; //Optional. Do later 
         }

      } else {
        
        if (noOfArgs(address) == 2) {
          return casePostIndexedAdSpec(atoi(ptr), getRm1(address), getExpr(address));
        }
      }

    default : return 0;
 }
}


int main (int argc, char** argv) {
 /* 
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
*/  
    //uint32_t example = ldr("r5", "[r10, #0xfff]");
    uint32_t ex2 = getExpr("[r19, #0xfff]");
    printf("0x%08x\n", ex2);
    printf("works\n");
    return 0;
}

