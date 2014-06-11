/*
 * branch.h
 *
 *  Created on: 2 Jun 2014
 *      Author: na4113
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include "macros.h"

uint32_t getVal(uint32_t inst, int left, int right) {

  int m = ((left - right) + 1);
  uint32_t result = ~(~0 << m) << right;
  inst &= result;
  inst >>= right;
  
  return inst;
  
}

//  These are the bits '1010' which are present in all branch instructions.
uint32_t code = 0xa << 24;

uint32_t asr(uint32_t ius, int n) {  // Method for arithmetically shifting to the right

	uint32_t nih = 1 << 31;          // The typical test binary number...

	for (int i = 0; i < n; i++) {    // Shifting one bit at a time as many times
		ius >>= 1;                   // as required (safer than all in one go).

		if ((ius & nih) != 0) {     // If the 32nd bit is 1,
		    ius += nih;              
		}
	}
	return ius;
}

uint32_t beq(char* label) { 
    uint32_t cond = 0x0 << 28;  //  insignificant but added for consistency.
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);

	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t bne(char* label) {
    uint32_t cond = 0x1 << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
//        if (offset < 0) 
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	
        printf("%08x\n", cond);
        printf("%08x\n", code); 
        printf("%08x\n", offset);
        printf("%08x\n",  asr(offset, 2)); 
        printf("%08x\n", cond + code + asr(offset, 2)); 
	return cond + code + asr(offset, 2) ;
}

uint32_t bge(char* label) {
    uint32_t cond = 0xa << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t blt(char* label) {
    uint32_t cond = 0xb << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t bgt(char* label) {
    uint32_t cond = 0xc << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t ble(char* label) {
    uint32_t cond = 0xd << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t b(char* label) {
    uint32_t cond = 0xe << 28;
	uint32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}


