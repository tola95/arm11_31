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
#include "dataStructures.h"

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
    cond = 0x0 << 28;  //  insignificant but added for consistency.
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t bne(char* label) {
    cond = 0x1 << 28;
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return cond + code + asr(offset, 2) + (1 << 28);
}

uint32_t bge(char* label) {
    cond = 0xa << 28;
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t blt(char* label) {
    cond = 0xb << 28;
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t bgt(char* label) {
    cond = 0xc << 28;
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t ble(char* label) {
    cond = 0xd << 28;
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}

uint32_t b(char* label) {
    cond = 0xe << 28;
	int32_t offset = lookup(&labels, label) - ((lineNumber*4) + 8);
	if (offset < 0) {
        offset = getVal(offset, 25, 0);  //  Same value but 26 bits.
	}
	return cond + code + asr(offset, 2);
}


