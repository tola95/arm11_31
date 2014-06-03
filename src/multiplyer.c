/*
 * multiplyer.c
 *
 *  Created on: 2 Jun 2014
 *      Author: na4113
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <SymbolTable.h>




/**
 * We could make the main method scan the assembly instruction so that the
 * number of the register specified is passed in as a parameter of each
 * method below, and therefore it could be interpreted as a binary number which
 * can be shifted as shown in their implementation below.
 */

uint32_t multiply(uint32_t rd, uint32_t rm, uint32_t rs) {
	uint32_t result = (1110 << 28) + (1001 << 4) + (rd << 16) + (rs << 8) + rm;
	return result;
}

uint32_t multiply_acc(uint32_t rd, uint32_t rm, uint32_t rs, uint32_t rn) {
	uint32_t result = (1110 << 28) + (1001 << 4) + (1 << 21)
			+ (rd << 16) + (rs << 8) + rm + (rn << 12);
	return result;
}
