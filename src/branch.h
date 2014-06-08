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


#define PC_    ARMReg[15].reg

uint32_t code = 0xa << 24;

uint32_t asr(uint32_t ius, int n) {  // Method for arithmetically shifting to the right

	uint32_t nih = 1 << 31;          // The typical test binary number...

	for (int i = 0; i < n; i++) {    // Shifting one bit at a time as many times
		ius >>= 1;                   // as required (safer than all in one go).

		if ((ius & nih) != 0) {        // If the 32nd bit is 1,
		    ius += nih;              //
		}
	}
	return ius;
}

uint32_t beq(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2);
}

uint32_t bne(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2) + (1 << 28);
}

uint32_t bge(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2) + (10 << 28);
}

uint32_t blt(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2) + (11 << 28);
}

uint32_t bgt(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2) + (12 << 28);
}

uint32_t ble(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
	return code + asr(offset, 2) + (13 << 28);
}

uint32_t b(char* label) {
	uint32_t offset = PC_ - 2 - lookup(&labels, label);
		return code + asr(offset, 2) + (14 << 28);
}


