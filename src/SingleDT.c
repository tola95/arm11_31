#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Temporary stuff to ensure compilation

char inst[] = "ldr";// Main should pass me inst(the instruction) <ldr/str>
uint32_t memPtr[16];// Memory or wherever we'll be storing the assembled code
int newaddress = 8; // Next available address after the assembled program
int currentaddress = 0; //Current location of the assembled program

//Single Data Transfer Functions

/**********************************************************************/         
/********Helper Functions to retrieve Rm, expression and Rn************/
/**********************************************************************/

 uint32_t getRn1(char *address) {
   //Gets the register number for registers of form [Rn]
   const char *p1 = strstr(address, "[r")+2;
   const char *p2 = strstr(p1, "]");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return atoi(res);
 }

uint32_t getRn(char *address) {
   //Gets the register number for registers of form [Rn,...
   const char *p1 = strstr(address, "[r")+2; 
   const char *p2 = strstr(p1, ",");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return atoi(res);
 }

 uint32_t getExpr(char *address) {
  //Gets expression of for #expression
   const char *p1 = strstr(address, "#")+1;
   const char *p2 = strstr(p1, "]");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return strtol(res, NULL, 16);
 }

 uint32_t getExpr1(char *address) {
  //Gets expression of for #expression
   const char *p1 = strstr(address, "#")+1;
   const char *p2 = strstr(p1, "\0");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return strtol(res, NULL, 16);
 }

 uint32_t getRm(char *address) {
   //Gets Rm from the optional cases
   const char *p1;
   if (strstr(address, "+") != NULL ) {
     p1 = strstr(address, "+")+2; 
   } else {
     p1 = strstr(address, "-")+2;
   }
   const char *p2 = strstr(p1, ",");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   return atoi(res);
 }
/*
 uint32_t getShift(char *address) {
   
 }

 uint32_t getShift1(char *address) {

 }
*/

 int noOfArgs(char *address) { //Gets number of args, necessary to distinguish between <address> cases
   if(strstr(address, ",") == NULL) {
     return 1;
   } else if (strstr(address, "+") == NULL && strstr(address, "-") == NULL ) {
     return 2;
   } else {
     return 3;
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
  

  if (address > 255) {
    
    //Put value of expression in four bytes at the end of the assembled program : ToDo
    memPtr[newaddress] = address; //newaddress is the next available adress at the end of the assembled program
    
    //Use this address, with PC as base reg and calculated offset, in assembled ldr instrction
    result |= (15 << 16); //with PC as base reg (rn)
    offset = newaddress - currentaddress;  //Compute the offse between current location and newly created one
                                           //Current adress is the address of this current instruction we're decoding
    result |= offset;
    result |= (1 << 24); //Always pre index 
    result |= (1110 << 28); //Cond bits always 'al'
    
    return result;

  } else {
    
    return 0;//mov(rd, address);
  }
}

uint32_t casePreIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression) {
  //Pre indexed address specification
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
  result |= (1110 << 28); //Cond bits always 'al'
  return result;
}

uint32_t casePostIndexedAdSpec( uint32_t rd, uint32_t address , uint32_t expression) {
  //Post indexed address specification
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
  result |= (1110 << 28); //Cond bits always 'al'
  return result;
}

/**************************************************************/
/*******************LDR and STR Functions**********************/
/**************************************************************/

uint32_t ldr(char* rd,  char* address) { 
  char *ptr = &rd[1];
  char *ad = &address[3];
  switch(address[0]) {
    // Case where address is =0x....
    case '='  :  return caseNumericConstant(atoi(ptr), strtol(ad, NULL, 16));

    //Case where address is [rn] , [rn, #expression] or [rn,{+/-}rm{,<shift>}]
    case '['  :  

      if (strstr(address, "],")==NULL) { //String "]," never occurs in pre index but always occurs in post index

        switch (noOfArgs(address)) {

           case 1 : return casePreIndexedAdSpec(atoi(ptr), getRn1(address), 0); //<ldr/str> rn [rm]

           case 2 : return casePreIndexedAdSpec(atoi(ptr), getRn(address), getExpr(address)); //<ldr/str> rn [rm, #0x..]

           case 3 : return 0; //Optional. Do later

           default : return 0; //Should never reach this
        }

      } else {
        
         switch (noOfArgs(address)) {

           case 2 : return casePostIndexedAdSpec(atoi(ptr), getRn1(address), getExpr1(address));//<ldr/str> rn [rm], #0x..

           case 3 : return 0; //Optional. Do later

           default : return 0; //Should never reach this

         }
      }
      
    
    default :  return 0;  
    
  } 
}

uint32_t str(char* rd,  char* address) {
   char *ptr = &rd[1];
  
   if (strstr(address, "],")==NULL) { //String "]," never occurs in pre index but always occurs in post index

         switch (noOfArgs(address)) {

           case 1 : return casePreIndexedAdSpec(atoi(ptr), getRn1(address), 0);

           case 2 : return casePreIndexedAdSpec(atoi(ptr), getRn(address), getExpr(address));

           case 3 : return 0; //Optional. Do later

           default : return 0; //Should never reach this

         }

   } else {

         switch (noOfArgs(address)) {

           case 2 : return casePostIndexedAdSpec(atoi(ptr), getRn1(address), getExpr1(address));

           case 3 : return 0; //Optional. Do later

           default : return 0; //Should never reach this

         }
          
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
    uint32_t example = ldr("r2", "=0x20200020");
    //uint32_t ex2 = getExpr("[r19, #0xfff]");
    printf("0x%08x\n", example);
    printf("works\n");
    return 0;
}

