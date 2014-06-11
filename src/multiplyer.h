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




/**
 * We could make the main method scan the assembly instruction so that the
 * number of the register specified is passed in as a parameter of each
 * method below, and therefore it could be interpreted as a binary number which
 * can be shifted as shown in their implementation below.
 */

uint32_t regValue(char *r) {
  char *val = &r[1];
  return atoi(val);
}

uint32_t multiply(char *rd, char *rm, char *rs) {
	uint32_t result = (0xe << 28) + (0x9 << 4) + (regValue(rd) << 16) + (regValue(rs) << 8) + regValue(rm);
	return result;
}

uint32_t multiply_acc(char *rd, char *rm, char *rs, char *rn) {
	uint32_t result = (0xe << 28) + (0x9 << 4) + (1 << 21)
			+ (regValue(rd) << 16) + (regValue(rs) << 8) + regValue(rm) + (regValue(rn) << 12);
	return result;
}
