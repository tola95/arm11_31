#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*  ---------------------------------------------------------------------
 *                            MACROPROCESSOR
 *  ---------------------------------------------------------------------
 *
 *  These macros make it easier to reference the registers, leading to prettier code.
 */ 
 
#define Rg(X)  ARMReg[(X)].reg
#define SP_    ARMReg[13].reg
#define LR_    ARMReg[14].reg
#define PC_    ARMReg[15].reg
#define CPSR_  ARMReg[16].reg




