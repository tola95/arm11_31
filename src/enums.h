#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*  ---------------------------------------------------------------------
 *                              ENUMERATIONS
 *  ---------------------------------------------------------------------
 */

 //  Constants for the number of registers and the number of byte addresses.
enum {REG = 17, MEM = 65536, MNEMONICS = 23};

enum bool {F, T};

//  Enumeration of instruction types. Useful when decoding instructions.
enum instructionType {

    DATA_PROCESSING, MULTIPLY, SINGLE_DATA_TRANSFER, BRANCH, SPECIAL   
} ;

//  Enumeration of all operation mnemonics for help with decoding and execution.
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
