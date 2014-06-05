#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "arm11Mem.h"



/*  ---------------------------------------------------------------------
 *                   DATA STRUCTURES USED IN EMULATOR
 *  ---------------------------------------------------------------------
 *
 *  The structs that store the information of the fetched and decoded
 *  instructions in emulate.c .
 */

//  Struct representing fetched instruction.
struct fetchedInstruction {

        uint32_t binaryInstruction; //  32-bit string representing the whole instruction.
        enum bool pending;          /*  Set to indicate that the currently fetched instruction should be
                                     *  decoded in the next cycle of the 'pipeline' in main.
                                     */ 
} ;
//  A pointer to an instance of the fetchedInstruction struct is declared.
//  It is declared outside of main because it needs to be a global variable.
struct fetchedInstruction *fetched;

//  Struct representing the data from decoded instructions.
struct decodedInstruction {

    enum mnemonic operation;  //  The opcode of the instruction to execute.  
    uint32_t offset;          //  These are all the potential flags and
    uint32_t cond;            //  values which may be required by an
    uint32_t op2;             //  instruction.
    uint32_t rd;              //
    uint32_t rn;              //  When an instruction is decoded,
    uint32_t rs;              //  the relevant fields are updated.
    uint32_t rm;               
    uint32_t i;
    uint32_t a;
    uint32_t s;
    uint32_t u;
    uint32_t p;
    uint32_t l;
    enum bool pending;          /*  Set to indicate that the currently decoded instruction should be
                                 *  executed in the next cycle of the 'pipeline' in main.
                                 */ 
} ;
//  A pointer to an instance of the decodedInstruction struct is declared.
//  It is declared outside of main because it needs to be a global variable.
struct decodedInstruction *decoded;
 

/*  Called by main to initialise the structs for the
 *  fetched instructions and the decoded instructions.
 */
void initdf(void) {

      decoded = malloc(sizeof(struct decodedInstruction));
      fetched = malloc(sizeof(struct fetchedInstruction));       
}



/*  ---------------------------------------------------------------------
 *                DATA STRUCTURES USED IN ASSEMBLER
 *  ---------------------------------------------------------------------
 *
 *  Includes the implementation of the symbol table which will be used to
 *  store the enum version of the opcodes in order to decode the string
 *  tokens in a switch statement.
 *  The symbol table will be a list of symbolTableElems.
 *  Follows one of the list structure from lecture 5;
 */
 
struct symbolTableElem {

    struct symbolTableElem *next;
    struct symbolTableElem *prev;
    int    value;
    char  *string;
};

struct symbolTable {

    struct symbolTableElem *header;
    struct symbolTableElem *footer;
};

//  Allocates memory for an individual symbolTableElem.
struct symbolTableElem *symbolTableElem_alloc(void) {

    //  Arbitrarily assume a label is never longer than 100 characters.
    size_t maxStringSize = sizeof(char)*100; 
    struct symbolTableElem *elem = malloc(sizeof(struct symbolTableElem) + maxStringSize);
    
    if (elem == NULL) {
    
        perror("Failed to allocate memory for symbol table element");
        exit(EXIT_FAILURE);
    }

    return elem;
}

//  Initialises a symbolTable.
void symbolTable_init(struct symbolTable *st) {

    st->header = symbolTableElem_alloc();
    st->footer = symbolTableElem_alloc();
    st->header->prev = NULL;
    st->footer->next = NULL;
    st->header->next = st->footer;
    st->footer->prev = st->header;
}


//  Defines an iterator for symbol tables.
typedef struct symbolTableElem* symbolTableIterator; 

//  Gets first element of a symbol table.
symbolTableIterator table_start(struct symbolTable *st) {
    return st->header->next;
}

//  Gets last element of a symbol table.
symbolTableIterator table_end(struct symbolTable *st) {
    return st->footer;
}


/*  ---------------------------------------------------------------------
 *                        SYMBOL TABLE METHODS
 *  ---------------------------------------------------------------------
 *
 */
 
//  Inserts an element to the end of the symbol table list.
void symbolTableInsert(struct symbolTable *st, char *string, int value) {

    //  Creates the new element to be inserted.
    struct symbolTableElem *newElem = symbolTableElem_alloc();
    newElem->string = string;
    newElem->value  = value;

    //  Inserts the new element at the end of the symbol table.
    symbolTableIterator iter = table_end(st);
    
    newElem->prev    = iter->prev;
    newElem->next    = iter;
    iter->prev->next = newElem;
    iter->prev       = newElem;
}

//  This function should only be called when the string is a known member of the symbolTable.
int lookup(struct symbolTable *st, char *string){

    struct symbolTableElem *elem = table_start(st);

    while (elem != NULL) {

        if ( !strcmp(elem->string, string) ) {

            return elem->value;
        }
        
        elem = elem->next;
    }
    
    return 0;
}

//  Global declarations so they can be referenced anywhere in the program.
struct symbolTable opCodes;
struct symbolTable labels;

/*  Initialises the two symbol tables the program will need.
 *  This function is called in main at the start of the program's execution.
 */
 
void initST(void){

    symbolTable_init(&opCodes);
    symbolTable_init(&labels);

    //  An array containing all the mnemonics supported.
    char *mnemonicStrings[] = {
    
        "and", "eor", "sub", "rsb", "add", "tst", "teq", "cmp", "orr",
        "mov", "mul", "mla", "ldr", "str", "beq", "bne", "bge", "blt",
        "bgt", "ble", "b", "lsl", "andeq"
    };

    /*  Makes a symbol table containing all the mnemonics. Look up a string
     *  to do a switch statement on it.
     */

    for (int i = 0; i < MNEMONICS; i++) {

        symbolTableInsert(&opCodes, mnemonicStrings[i], i);
    }

}









