#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

enum mnemonic {  

    AND, EOR, SUB,
    RSB, ADD, TST,
    TEQ, CMP, ORR,
    MOV, MUL, MLA,
    LDR, STR, BEQ,
    BNE, BGE, BLT,
    BGT, BLE, B,
    LSL, ANDEQ
    
};

typedef struct symboltablerow {

        char *label;
        int   memoryAddress;
        
} symbolTableRow;

typedef struct mnemonictablerow {

        char *string;
        enum mnemonic opcode;
        
} mnemonicTableRow;

enum mnemonic lookUp(char *string) {

        for (int i = 1; i < len(mnemonicTable); i++){
                if (strcmp(mnemonicTable[i]->string, string)){
                        return mnemonicTable[i]->opcode;
                }
        }
        return 0; //Never reaches here
}

typedef struct deconstruct {
     
    char *rn;
    char *rd;
    char *operand2;
    char *rs;
    char *rm;
} Decon;

Decon *pieces;

const char limits[3] = ", ";

void setMnemonic(char *instruction) {

  enum mnemonic opcode = lookUp(instruction);
 
  switch(opcode) {
  case AND :
    printf("YAY");
  case EOR :
    printf("YO");
  }
}

void seperate(char *instruction) {
  printf("%s\n", pieces->operand2);
  char *result;
  result = strtok(instruction, limits);
  pieces->rn = instruction;
  result = strtok(NULL, limits);
  printf("%s\n", pieces->rn);
}


int main() { 
  /*
  pieces = malloc(sizeof(Decon)); 
  char inst[] = "Hello there! This is an instruction.";
  char *result;
  result = strtok(inst, limits);
  printf("Here\n");
  pieces->opcode = result;
  result = strtok(NULL, limits);
  seperate(result);*/
  char *ins = "and";
  setMnemonic(ins);
  return 0;
}

/*
 int main(int argc, char **argv) {

		 FILE *file = fopen(argv[1], "r");
		 printf("works\n");

		 return EXIT_SUCCESS;

 }
 */
