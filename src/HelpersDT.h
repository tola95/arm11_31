#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int minus = 0;
int isRegister = 0;
int pre = 0;
int shift;

//Single Data Transfer Functions

/**********************************************************************/         
/********Helper Functions to retrieve Rm, expression and Rn************/
/**********************************************************************/

 uint32_t getRn1(char *address) {
   //Gets the register number for registers of form [Rn]
   char a[] = "[r";
   char b[] = "]";
   const char *p1 = strstr(address, a)+2;
   const char *p2 = strstr(p1, b);
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
   if (strstr(address, "#")!=NULL) {
     const char *p1;
     if (strstr(address, "x") == NULL) {
       p1 = strstr(address, "#")+1;
     } else {
       if (strstr(address, "-")==NULL) {
         p1 = strstr(address, "#")+3;
       } else {
         minus = 1;
         p1 = strstr(address, "#")+4;
       }
     }
     const char *p2 = strstr(p1, "]");
     size_t len = p2-p1;
     char *res = (char*)malloc(sizeof(char)*(len+1));
     strncpy(res, p1, len);
     res[len] = '\0';
     return atoi(res);
   } else {
     const char *p1 = strstr(address, ",r")+2; 
     const char *p2 = strstr(p1, "]");
     size_t len = p2-p1;
     char *res = (char*)malloc(sizeof(char)*(len+1));
     strncpy(res, p1, len);
     res[len] = '\0';
     isRegister = 1;
     return atoi(res);
   }
   
 }

 uint32_t getExpr1(char *address) {
  //Gets expression of for #expression
   if (strstr(address, "#")!=NULL) {
   const char *p1;
   if (strstr(address, "x") == NULL) {
     p1 = strstr(address, "#")+1;
   } else {
     if (strstr(address, "-")==NULL) {
       p1 = strstr(address, "#")+3;
     } else {
       minus = 1;
       p1 = strstr(address, "#")+4;
     }
   }
  
   return atoi(p1);
   } else {
   const char *p1 = strstr(address, ",r")+2; 
  
   isRegister = 1;
   return atoi(p1);
   }
 }

 uint32_t getRm(char *address) {
   
   //Gets Rm from the optional cases
   const char *p1;
   if (strstr(address, "-") != NULL ) {
     p1 = strstr(address, "-")+2; 
   } else {
     p1 = strstr(address, "+")+2;
   }
   const char *p2 = strstr(p1, ",");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   printf("Here\n");
   return atoi(res);
 }

uint32_t getExpr2(char *address) {
  if (strstr(address, "#")!=NULL) {
  const char *p1 = strstr(address, "#")+1;
   const char *p2 = strstr(p1, "]");
   size_t len = p2-p1;
   char *res = (char*)malloc(sizeof(char)*(len+1));
   strncpy(res, p1, len);
   res[len] = '\0';
   
   return atoi(res);
   } else {
   isRegister = 1;
   return 0;
   }
}

 int count_chars(const char* string, char *ch)
{
    int count = 0;
    int i;

    int length = strlen(string);

    for (i = 0; i < length; i++)
    {
        if (string[i] == ch[0])
        {
            count++;
        }
    }

    return count;
}

 int noOfCommas(char *address) {
   char *comma = ",";
   return count_chars(address, comma);
}

 int noOfArgs(char *address) { //Gets number of args, necessary to distinguish between <address> cases
   if(strstr(address, ",") == NULL) {
     return 1;
   } else if ( noOfCommas(address) == 1){ //strstr(address, "+") == NULL && strstr(address, "-") == NULL
     return 2;                            //noOfCommas(address) == 1
   } else {
     return 3;
   }
 }
