#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dataStructures.h"


/*  ---------------------------------------------------------------------
 *                      FUNCTIONS CALLED BY MAIN
 *  ---------------------------------------------------------------------
 */

 
//  Adds a string to a symbolTable with its appropriate memoryAddress.
void addToSymbolTable(char *label, int lineNumber, int labelCount){

    uint32_t memAddress = (lineNumber - labelCount) * 4;
}

//  Checks if a string is a label (I.E. contains a ':').
enum bool lineIsLabel(char *line){

    int lineLength = strlen(line);         //  Number of characters to iterate through.

    for (int i = 1; i < lineLength; i++){  //  First character (line[0]) can't be ':'.

        if (line[i] == ':'){

            return T;                     //  returns True if ':' is found.
        }
    } 

    return F;
}


/*  ---------------------------------------------------------------------
 *                           MAIN FUNCTION
 *  ---------------------------------------------------------------------
 */
int main(int argc, char **argv) {

    /*  Initialise ARM memory where binary instructions will be stored
     *  after they have been converted from assembly.
     */
    initMem();

    //  Initialise the symbol tables used by the assembler.
    initST();

    //  Open the file containing the assembly code.
    FILE *file = fopen(argv[1], "r");
    if (file == NULL){
             perror( " Error: Could not open file. \n" );
             exit(EXIT_FAILURE);
    }

    //  The name to be given to the binary output file.
    char *outputFileName = argv[2];

/*  ---------------------------------------------------------------------
 *                               FIRST PASS
 *  ---------------------------------------------------------------------
 *
 *  The assembler uses "two-pass" assembly. This is the first pass in which all
 *  labels are stored in a symbolTable with their corresponding memory addresses.
 */
     
    int   lineNumber =  0;  //  Must keep track of line number to calculate memory addresses.
    int   labelCount =  0;  //  Must keep track of number of labels to calculate memory addresses.
    char  line[100];        /*  This variable stores the previously read line from the file.
                             *  Assumes a line is never longer than 100 characters in length.
                             */

    
    /*  fgets returns NULL pointer when it reaches the end of the file.
     *  100 is an arbitrary number much larger than the maximum expected
     *  length of a line of assembly code.
     *  fgets stores the next line in the 'line' variable.
     */

    while( fgets(line, 100, file ) != NULL){  //  Loops until the end of the file.

        lineNumber++;          //  Increase line number with each iteration.
        
        if ( lineIsLabel(line) ){  //  Add the line to the symbol table if it is a label.

            addToSymbolTable(line, lineNumber, labelCount);
            labelCount++;      //  Indicates how many labels have been found so far.
            
        }
        
    }

    //  Return to the start of the file in preparation for the second pass.
    fseek( file, 0, SEEK_SET );

/*  ---------------------------------------------------------------------
 *                             SECOND PASS
 *  ---------------------------------------------------------------------
 *
 *  The assembler uses "two-pass" assembly. This is the second pass in which 
 *  the lines are decoded and converted into binary instructions which are
 *  then stored in a memory array.
 */

    while( fgets(line, 100, file ) != NULL){  //  Loops until the end of the file.
    
    }


        return EXIT_SUCCESS;

}





