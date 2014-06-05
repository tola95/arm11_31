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

    //  Calculates memory address from line number and number of previous labels.
    uint32_t memAddress = (lineNumber - labelCount) * 4;
    symbolTableInsert(&labels, label, memAddress);
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

//  Used to store a machine code value in the specified memory address.
void putInMem(uint32_t memAddr, uint32_t value) {
    
    uint8_t val = 0;
    uint32_t mask = 0xff000000;
        
    for (int i = 0; i < 4; i++) {              //  Splits the 32bit value into 4 values of size 1 byte each.
        val = (value & mask) >> ((3 - i)*8) ;  //  Then stores these bytes in 4 consecutive byte addresses.
        memPtr[memAddr + i] = val;
        mask >>= 8;
    }

    
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
    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL){
             perror( " Error: Could not open file. \n" );
             exit(EXIT_FAILURE);
    }


/*  ---------------------------------------------------------------------
 *                               FIRST PASS
 *  ---------------------------------------------------------------------
 *
 *  The assembler uses "two-pass" assembly. This is the first pass in which all
 *  labels are stored in a symbolTable with their corresponding memory addresses.
 */
     
    int   lineNumber =  0;  //  Must keep track of line number to calculate memory addresses.
    int   labelCount =  0;  //  Must keep track of number of labels to calculate memory addresses.
    char  *line;            //  This variable stores the previously read line from the file.

    
    /*  fgets returns NULL pointer when it reaches the end of the file.
     *  100 is an arbitrary number much larger than the maximum expected
     *  length of a line of assembly code.
     *  fgets stores the next line in the 'line' variable.
     */

    while( fgets(line, 100, inputFile ) != NULL){  //  Loops until the end of the file.

        lineNumber++;              //  Increase line number with each iteration.
        
        if ( lineIsLabel(line) ){  //  Add the line to the symbol table if it is a label.

            addToSymbolTable(line, lineNumber, labelCount);
            labelCount++;          //  Indicates how many labels have been found so far.
            
        }
        
    }

    //  Return to the start of the file in preparation for the second pass.
    fseek( inputFile, 0, SEEK_SET );
    lineNumber = 0;

/*  ---------------------------------------------------------------------
 *                             SECOND PASS
 *  ---------------------------------------------------------------------
 *
 *  The assembler uses "two-pass" assembly. This is the second pass in which 
 *  the lines are decoded and converted into binary instructions which are
 *  then stored in a memory array.
 */

    while( fgets(line, 100, inputFile ) != NULL){  //  Loops until the end of the file.

        if (lineIsLabel(line)) continue;      //  Skips label lines.

        lineNumber++;

        /* Gets the token reprsenting the mnemonic of the line and uses lookup
         * to return its enum mnemonic equivalent.
         */  
        enum mnemonic opcode = lookup(&opCodes, strtok(line, " "));

        //  uint32_t to store the machine code from the assembled line.
        uint32_t machineCode;

        /*  Uses the opcode to decide how to decode the line and finds the
         *  the tokens needed for that particular operation type.
         */
         
        switch(opcode) {

            //  Branch instructions.
            case B    :  machineCode =   b(strtok(NULL, " ")) ; break;
            case BEQ  :  machineCode = beq(strtok(NULL, " ")) ; break;
            case BNE  :  machineCode = bne(strtok(NULL, " ")) ; break;
            case BGE  :  machineCode = bge(strtok(NULL, " ")) ; break;
            case BLT  :  machineCode = blt(strtok(NULL, " ")) ; break;
            case BGT  :  machineCode = bgt(strtok(NULL, " ")) ; break;
            case BLE  :  machineCode = ble(strtok(NULL, " ")) ; break;
            //  Data processing with single operand assignment.
            case MOV  :  machineCode = mov(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            //  Data processing, doesnt compute results.
            case TST  :  machineCode = tst(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case TEQ  :  machineCode = teq(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case CMP  :  machineCode = cmp(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            //  Data processing, computes results.
            case AND  :  machineCode = and(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case EOR  :  machineCode = eor(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case SUB  :  machineCode = sub(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case RSB  :  machineCode = rsb(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case ADD  :  machineCode = add(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case ORR  :  machineCode = orr(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            //  Multiply instructions.
            case MUL  :  machineCode = mul(strtok(NULL, " ,"), strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case MLA  :  machineCode = mla(strtok(NULL, " ,"), strtok(NULL, " ,"),
                                           strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            //  Single data transfer instructions.
            case LDR  :  machineCode = ldr(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            case STR  :  machineCode = str(strtok(NULL, " ,"), strtok(NULL, " ,")); break;
            //  For andeq.
            default   :  machineCode = 0; break;
            
        }

        //  Store the new machine code instruction in the memory array.
        putInMem((lineNumber*4), machineCode);
    
    }

    //  Close the file once all instructions have been read.
    fclose(inputFile);

    //  Create the output file. "ab" creates the file and opens for writing in binary mode.
    FILE *outputFile = fopen(argv[2], "ab");

    //  Writes as many lines as were decoded to prevent thousands of 0s being written.
    for (int i = 0; i < (lineNumber * 4); i++){

        fwrite(&memPtr[i], sizeof(uint8_t), 1, outputFile );

    }

    


        return EXIT_SUCCESS;

}





