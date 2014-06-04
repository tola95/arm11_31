/*
 * doublepointer.c
 *
 *  Created on: 2 Jun 2014
 *      Author: na4113
 */


#include <stdio.h>
#include <stdlib.h>


const int longitudo = 39;
char **weirdo = (char**) malloc(longitudo * sizeof(char*));


int main(int argc, char **argv) {

	weirdo = { "One", "Git-Git", "Felix the Cat", "Figaro", "The Git Tweeters" };

	printf(weirdo[3]);
	printf(*weirdo[3]);

	return EXIT_SUCCESS;

}
