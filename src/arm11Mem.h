#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "enums.h"

/*  ---------------------------------------------------------------------
 *                            ARM11 MEMORY
 *  ---------------------------------------------------------------------
 */


//  A struct for storing an individual register.
typedef struct ARM_REGISTER {
  char ident[4];
  uint32_t reg;
} Register;


/*  Creates an array of registers
 *  Each register is assigned a string naming the register
 *  and its value is initialised to 0.
 */
Register ARMReg[REG] = {  //  REG is a constant for the number of registers defined in enums.h
  {"$0",   0},
  {"$1",   0},
  {"$2",   0},
  {"$3",   0},
  {"$4",   0},
  {"$5",   0},
  {"$6",   0},
  {"$7",   0},
  {"$8",   0},
  {"$9",   0},
  {"$10",  0},
  {"$11",  0},
  {"$12",  0},
  {"SP",   0},
  {"LR",   0},
  {"PC",   0},
  {"CPSR", 0}
};

//  Creating a pointer to the ARM Machine main memory with global scope.
uint8_t *memPtr;


//  This function is called by main to initialise the emulator's memory when the emulator starts running.
void initMem(void) {

  memPtr = (uint8_t *) calloc(MEM, sizeof(uint8_t));  /*  MEM is a constant defined in enums.h
                                                       *  representing the maximum number of byte
                                                       *  addresses in the ARM11 memory
                                                       */
}

