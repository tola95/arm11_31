/*
 *  Created on: 22 May 2014
 *      Author: Group 31
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

/*  ---------------------------------------------------------------------
 *                            MACROPROCESSOR
 *  ---------------------------------------------------------------------
 *
 *  These macros make it easier to reference the registers, leading to prettier code.
 */ 
 
#define Rg(X)  ARMReg[(X)].reg
#define SP_    ARMReg[13].reg
#define LR_    ARMReg[14].reg
#define PC_    ARMReg[15].reg
#define CPSR_  ARMReg[16].reg


/*  ---------------------------------------------------------------------
 *                              ENUMERATIONS
 *  ---------------------------------------------------------------------
 */
 
//  Constants for the number of registers and the number of byte addresses.
enum {REG = 17, MEM = 65536};

//  Boolean type enumeration.
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


/*  ---------------------------------------------------------------------
 *                            DATA STRUCTURES
 *  ---------------------------------------------------------------------
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
    uint32_t rs;              //  the relevant fields are updated
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
 

//  A struct for storing an individual register.
typedef struct ARM_REGISTER {

  char ident[4];
  uint32_t reg;
} Register;

/*  Creates an array of registers
 *  Each register is assigned a string naming the register
 *  and its value is initialised to 0.
 */
Register ARMReg[REG] = {
  {"$0",   0},
  {"$1",   0},
  {"$2",   0},
  {"$3",   0},
  {"$4",   0},
  {"$5",   0},
  {"$6",   0},
  {"$7",   0},
  {"$8",   0},
  {"$9",   0},
  {"$10",  0},
  {"$11",  0},
  {"$12",  0},
  {"SP",   0},
  {"LR",   0},
  {"PC",   0},
  {"CPSR", 0}
};


/*  ---------------------------------------------------------------------
 *                           INITIALISATION
 *  ---------------------------------------------------------------------
 */
//  Creating a pointer to the ARM Machine main memory with global scope.
uint8_t *memPtr;

//  This function is called by main to initialise the emulator's memory when the emulator starts running.
void initMem() {
  memPtr = (uint8_t *) calloc(MEM, sizeof(uint8_t));
}

/*  Called by main to initialise the structs for the
 *  fetched instructions and the decoded instructions.
 */
void initdf(void) {

      decoded = malloc(sizeof(struct decodedInstruction));
      fetched = malloc(sizeof(struct fetchedInstruction));       
}

/*  ---------------------------------------------------------------------
 *                        LOADING/STORING DATA
 *  ---------------------------------------------------------------------
 */

//  Used to swap the endianess of an instruction being read from the memory.
//  This function is called any time an instruction must be read from the memory.
uint32_t fetchFromMem(uint32_t start) {
  
  uint32_t reversedInst = 0;

  for (int i = 0; i < 4; i++) {
  
    uint32_t val  = memPtr[start + i] << (i * 8);
    reversedInst |= val;

  }

  return reversedInst;

}

/*  Checks if loading from a GPIO address.
 *  The GPIO addresses are used by the raspberry pi
 *  for input/ouput but are not in the emulator's
 *  memory.
 */
enum bool checkGPIOLoad(uint32_t mem, uint32_t rd) { 

  if (mem == 0x20200000) {
    printf("One GPIO pin from 0 to 9 has been accessed\n");
    Rg(rd) = mem;
    return T;
  }
  if (mem == 0x20200004) {
    printf("One GPIO pin from 10 to 19 has been accessed\n");
    Rg(rd) = mem;
    return T;
  }
  if (mem == 0x20200008) {
    printf("One GPIO pin from 20 to 29 has been accessed\n");
    Rg(rd) = mem;
    return T;
  }
  
  return F;  //  Indicates the address being checked is not a GPIO address.
}

//  Used to store the value at a specific memory address in the desginated register.
void fromMemtoReg(uint32_t address, uint32_t rd) {

  if (!checkGPIOLoad(address, rd)){  //  Checks if loading from a GPIO address.
  
    if (address > MEM - 4) {         /*  MEM is the number of byte addresses.
                                      *  4 bytes are loaded in each call so
                                      *  MEM - 4 is the last address that can be loaded from.
                                      */
                               
        printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        return;  //  Ends the function call.
        
    }
    
    Rg(rd) = fetchFromMem(address);  //  Rg(X) is a macro defined earlier.
    
  }
  
}

/*  Checks if storing in a GPIO address.
 *  The GPIO addresses are used by the raspberry pi
 *  for input/ouput but are not in the emulator's
 *  memory.
 */
enum bool checkGPIOStore(uint32_t mem, uint32_t val) {

  if (mem == 0x20200000) {
    printf("One GPIO pin from 0 to 9 has been accessed\n");
    return T;
  }
  if (mem == 0x20200004) {
    printf("One GPIO pin from 10 to 19 has been accessed\n");
    return T;
  }
  if (mem == 0x20200008) {
    printf("One GPIO pin from 20 to 29 has been accessed\n");
    return T;
  }
  
  if (mem == 0x2020001c) {
    printf("PIN ON\n");
    return T;
  }
  if (mem == 0x20200028) {
    printf("PIN OFF\n");
    return T;
  }

  
  return F;  //  Indicates the address being checked is not a GPIO address.
}

//  Used to store a value in the designated memory address.
void putInMem(uint32_t memAddr, uint32_t value) {

    if (!checkGPIOStore(memAddr, value)) {   //  Checks if storing in a GPIO address.
    
        uint8_t val = 0;
        uint32_t mask = 0xff000000;
        
        for (int i=3; i>=0; i--) {           //  Splits the 32bit value into 4 values of size 1 byte each.
            val = (value & mask) >> (i*8) ;  //  Then stores these bytes in 4 consecutive byte addresses.
            memPtr[memAddr + i] = val;
            mask >>= 8;
        }
    }
    
}

/*  
    ============================================================================

This function is never called in the program !! This should be removed right ?

void printBits1(uint32_t x) {
  int i;
  uint32_t mask = 1 << 31;
  for(i=0; i<32; ++i) {
    if((x & mask) == 0)
      printf("0");
    else
      printf("1"); x = x << 1;
  }
  printf("\n "); }

    ===========================================================================
  
 */

/*  NOTE:
 *  The following functions use macros defined earlier to make code easier to read.
 *  '' Rg(x) '' is replaced with '' ARMReg[(x)].reg '' at compile time.
 *  (I.E. The register numbered x)
 *  ''PC_'',''SD_'',''LR_'' & ''CPSR_'' are similarly replaced with
 *  similar code pointing to the values in their respective registers.
 *  These are followed by underscores to avoid conflicts with other code.
 */


//  Creates a mask given the first and last index to mask from.
uint32_t getVal(uint32_t inst, int left, int right) {

  int m = ((left - right) + 1);
  uint32_t result = ~(~0 << m) << right;
  inst &= result;
  inst >>= right;
  
  return inst;
  
}

/*  ---------------------------------------------------------------------
 *                         SHIFT OPERATIONS
 *  ---------------------------------------------------------------------
 *
 *  These are used to shift the values of Operand2 when its not an immediate value.
 *  They are only called by other operations.
 */
 
uint32_t lsl(uint32_t rmVal, uint32_t shift) {  //  Logical shift left


//Opcode and operation for when operand2 is not an immediate constant.
uint32_t lsl(uint32_t rm, uint32_t shift) {
  //uint32_t carryOut = rm << shift - 1;
  uint32_t get = 31 - (shift - 1);
  
  if (decoded->s == 1)  {
  
    CPSR_ += ( (getVal(rmVal, get, get)) << (shift - 1)) >> 2;
   // CPSR_ += (getVal(rmVal, 31, 31)) << 29;     THIS SHOULD BE DELETED RIGHT ?
  }
  
  return rmVal << shift;
  
}

uint32_t lsr(uint32_t rmVal, uint32_t shift) {  //  Logical shift right

  if (decoded->s == 1) {
  
    CPSR_ += (getVal(rmVal, shift - 1, shift - 1)) >> (shift -1);
  }
  
  return rmVal >> shift;
  
}

uint32_t asr(uint32_t rmVal, uint32_t shift) {  //  Arithmetic shift right

  if (decoded->s == 1) {
  
  CPSR_ += (getVal(rmVal, shift - 1, shift - 1)) >> (shift -1);
  }
  
  uint32_t bit31 = rmVal & (1 << 31);
  
  while (shift > 0) {
  
    rmVal >>= 1;
    rmVal += bit31;
    shift --;
  }
  
  return rmVal;
  
}

uint32_t ror(uint32_t imm, uint32_t n) {  //  Rotate right

  while ( n > 0) {
  
    uint32_t zerobit = 1 & imm;
    zerobit <<= 31;
    imm >>= 1;
    imm += zerobit;
    n --;
  }
  
  return imm;
  
}

uint32_t rotate(uint32_t rmVal, uint32_t shift) {  //  Calls ror to rotate right

  if (decoded->s == 1) {
    CPSR_ += (getVal(rmVal, shift - 1, shift - 1)) >> (shift -1);
  }
  
  return ror(rmVal, shift * 2);
  
}

/*  ---------------------------------------------------------------------
 *                   HELPER FUNCTIONS FOR OPERATIONS
 *  ---------------------------------------------------------------------
 */

/*  Since the V bit is unaffected by the instruction, this funtion preserves the 28th bit of the CPSR
 *  and initialises the rest to zero.
 */
uint32_t setCPSRA() {

  CPSR_ &= (1 << 28);
  
  return CPSR_;
  
}

void setCPSRL(uint32_t result) {

    CPSR_ = setCPSRA();      //  First clears the CPSR flags (except V)
    
    if (result == 0) {
    
      CPSR_ += (1 << 30);    //  If the last result was 0, set Z.
    }                        //  Used when testing for equality.
    
    if ((result & (1 << 31)) == pow(2, 31)) {
    
      CPSR_ += (1 << 31);    //  Sets N depending on the sign of the last result.
    }                        //  Used when testing for inequality.
    
}

//  getOp2 calls this function if I is set
uint32_t iIsSet(uint32_t op2) {

  if (decoded->s == 1) {
  
    CPSR_ = setCPSRA();
  }
  
  //CPSR_ += (getVal(op2, 3, 3)) << 29;  THIS SHOULD BE DELETED RIGHT ?
  uint32_t imm = getVal(op2, 7, 0);
  uint32_t rot = getVal(op2, 11, 8);
  
  return rotate(imm, rot);
    
}

// getOp2 calls this function if I is not set
uint32_t iIsNotSet(uint32_t op2) {

  if (decoded->s == 1) {
  
    CPSR_ = setCPSRA();
  }
  
  uint32_t shift = getVal(op2, 11, 4);
  uint32_t rm = op2 & 15;                 //  gives the last 4 bits of op2.
  uint32_t shiftVal = getVal(op2, 11, 7);
  uint32_t shiftType = getVal(op2, 6, 5);
  
  if ((shift & 1) == 1) {
  
    shiftVal = Rg(getVal(op2, 11, 8));
  }
  
  switch(shiftType) {
  
    case 0  :  return lsl(Rg(rm), shiftVal);
    
    case 1  :  return lsr(Rg(rm), shiftVal);
    
    case 2  :  return asr(Rg(rm), shiftVal);
    
    case 3  :  return ror(Rg(rm), shiftVal);
    
    default :  return 0;  //  Should never reach the default case.
    
  }
}

//  Called to get the correct Op2 based on the I flag.
uint32_t getOp2(void) {

  uint32_t operand2 = decoded->op2;
  
  switch(decoded->i) {
  
    case 0  :  return iIsNotSet(operand2);
    
    case 1  :  return iIsSet(operand2);
    
    default :  return 0;  //  Should never reach the default case.
    
  }
}
 
/*  ---------------------------------------------------------------------
 *                    DATA PROCESSING OPERATIONS
 *  ---------------------------------------------------------------------
 */

uint32_t tst(uint32_t rn, uint32_t op2) {  //  CPSR flags := Rn AND Op2

  uint32_t result = rn & op2;
  
  if (decoded->s == 1) {
    setCPSRL(result);
  }
  
  return result;
  
}

uint32_t teq(uint32_t rn, uint32_t op2) {  //  CPSR flags := Rn XOR Op2

  uint32_t result = rn ^ op2;
  
  if (decoded->s == 1) {
    setCPSRL(result);
  }
  
  return result;
  
}

uint32_t cmp(uint32_t rn, uint32_t op2) {  //  CPSR flags := Rn - Op2

  uint32_t result = rn - op2;
  
  if (decoded->s == 1) {
    setCPSRL(result);
    
    if (rn >= op2) {
      CPSR_ += (1 << 29);  //  Updates the C flag in the CPSR
    }
  }
  
  return result;
  
}

void and(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Rn AND Op2
  Rg(rd) = tst(rn, op2);
}

void eor(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Rn XOR Op2
  Rg(rd) = teq(rn, op2);
}

void sub(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Rn - Op2
  Rg(rd) = cmp(rn, op2);
}

void rsb(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Op2 - Rn
  Rg(rd) = cmp(op2, rn);
}

void add(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Rn + Op2
  
  uint64_t carryChecker = rn + op2;
  Rg(rd) = rn + op2;
  
  if (decoded->s == 1) {
    setCPSRL(Rg(rd));
    
    if (carryChecker > ( pow(2,32) - 1 )){
      CPSR_ += 1 << 29;  //  Updates the C flag in the CPSR
    }
  }
  
}

void orr(uint32_t rn, uint32_t op2, uint32_t rd) {  //  Rd := Rn OR Op2

  Rg(rd) = rn | op2;
  
  if (decoded->s == 1) {
    setCPSRL(Rg(rd));
  }
  
}

void mov(uint32_t op2, uint32_t rd) {  //  Rd := Op2

  Rg(rd) = op2;
  
  if (decoded->s == 1) {
    setCPSRL(Rg(rd));
  }
  
}

/*  ---------------------------------------------------------------------
 *                        MULTIPLY OPERATIONS
 *  ---------------------------------------------------------------------
 *
 *  The function checks to see if the A flag is set and acts accordingly
 *  so only one function is required for both multiply operations.
 */
 
void multiply(void) {

void multiply(void) {
  uint32_t result = Rg(decoded->rm) * Rg(decoded->rs);
  
  if (decoded->a == 1) {   //  executed if MLA operation.
  
    result += Rg(decoded->rn);
  }
  
  Rg(decoded->rd) = result;
  setCPSRL(result);        //  THIS IS REDUNDANT AND SHOULD BE DELETED RIGHT ?
  
  if (decoded->s == 1) {
    setCPSRA();            //  Clear CPSR (Except V bit)
    
    if (result == 0) {
    
      CPSR_ += (1 << 30);  //  Set Z flag in CPSR if result was zero
    }
    
    if ((result & (1 << 31)) == pow(2, 31)) {
    
      CPSR_ += (1 << 31);  //  Set N flag in CPSR if result was negative
    };
  }
}

 

    PC_ += 4;//PC_ is a macro defined earlier

/*  ---------------------------------------------------------------------
 *                        SINGLE DATA OPERATIONS
 *  ---------------------------------------------------------------------
 */
void ldr(uint32_t arg) {

    uint32_t offset;
    uint32_t result;

    //  If I is set, arg is interpreted as a shift register, otherwise 12 bit offset
    switch (decoded -> i) {
        case 1  :  offset = iIsNotSet(arg);
        break;
                           
        case 0  :  offset = arg;
        break;
                           
        default :  break;
    }

    //  If P is set, offset is added/subtracted (depending on u flag) before transferring data
    if (decoded -> p == 1) {

        if (decoded -> u == 1) {
        
            result = Rg(decoded -> rn) + offset;
            
        } else {
        
            result = Rg(decoded -> rn) - offset;
        }
        
        //  Data is transferred afterwards
        fromMemtoReg(result, decoded->rd);
                
    } else {
    
        //  Data is transferred beforehand 
        fromMemtoReg(offset, decoded->rd);

        if (decoded -> u == 1) {
        
            Rg(decoded -> rn) += offset;
            
        } else {
        
            Rg(decoded -> rn) -= offset;
            
        }     
    }     
}

void str(uint32_t arg) {
       
    uint32_t offset;
    uint32_t result;
        
    //  If I is set, arg is interpreted as a shift register, otherwise 12 bit offset
    switch (decoded -> i) {
          
        case 1   :  offset = iIsNotSet(arg);
                    break;
                
        case 0   :  offset = arg;
                    break;
                
        default  :  break;
    }

    //  If P is set, offset is added/subtracted (depending on u flag) before transferring data
    if (decoded -> p == 1) {
    
        //If u is set, offset is added
        if (decoded -> u == 1) {
        
            result = Rg(decoded -> rn) + offset;
            
        } else {
        
            result = Rg(decoded -> rn) - offset;
        }
        
        //  Data is transferred afterwards
        putInMem(result, Rg(decoded -> rd));
        
    } else {
    
        //  Data is transferred beforehand
        putInMem(offset , Rg(decoded->rd));

        if (decoded -> u == 1) {
        
            Rg(decoded -> rn) += offset;
            
        } else {
        
            Rg(decoded -> rn) -= offset;
        }
    }
}

/*  ---------------------------------------------------------------------
 *                         BRANCH OPERATIONS
 *  ---------------------------------------------------------------------
 *
 *  Executed if the conditions match the CPSR (I.E. checkCond(instruction) == T)
 *  The conditions are checked before the branch operation is called so only
 *  one branch function is required for all branch operations.
 */
 
void branch(uint32_t offset) {

  uint32_t sign = getVal(offset, 23, 23);
  offset <<= 2 ;
  offset &= 0x03ffffff;
        
  if (sign == 1) {
        
     offset += 0xfc000000;
  }
                
  PC_ += offset;
  (fetched -> pending) = F;
  (decoded -> pending) = F;
  
}


/*  ---------------------------------------------------------------------
 *                      FUNCTIONS CALLED BY MAIN
 *  ---------------------------------------------------------------------
 */
 
//  function to increment the current instruction address.
void incrementPC(void){

    PC_ += 4;  //  PC_ is a macro defined earlier
}

enum bool checkFlag(char flag) {

  uint32_t mask;
  
  switch (flag) {
  
    case 'N' : mask = 0x80000000; break;
    
    case 'Z' : mask = 0x40000000; break;
    
    case 'C' : mask = 0x20000000; break;
    
    case 'V' : mask = 0x10000000; break;
    
    default  : return F;  //  Should never reach default case.
    
  }
  
  return (CPSR_ & mask);
  
}

//  Function to check whether the conditions match the CPSR
enum bool checkCond() {

  switch (decoded->cond) {
  
    case 0   : return checkFlag('Z');
    
    case 1   : return !checkFlag('Z');
    
    case 10  : return checkFlag('N') == checkFlag('V');
    
    case 11  : return checkFlag('N') != checkFlag('V');
    
    case 12  : return !checkFlag('Z') && (checkFlag('N') == checkFlag('V'));
    
    case 13  : return checkFlag('Z') | (checkFlag('N') != checkFlag('V'));
    
    case 14  : return T;  //  Instruction is execute regardless of CPSR
    
    default  : return F;  //  Should never reach default case.
 
  }
}

/*  ---------------------------------------------------------------------
 *                              FETCH
 *  ---------------------------------------------------------------------
 *
 *  Function to fetch the instruction at the current byte address stored in PC.
 */
 
void fetchNextInstruction(void){

    fetched->binaryInstruction = fetchFromMem(PC_); //PC_ is a macro defined earlier.
    fetched->pending = T; 
}
 
//function to execute decoded instructions and clear decoded instructions pending flag.
void executeDecodedInstruction(void){


/*  ---------------------------------------------------------------------
 *                            DECODE
 *  ---------------------------------------------------------------------
 */
 
//  Helper function to figure out the operation mnemonic from the 32-bit instruction.
enum instructionType mnemonicDecoder(void){

    /*  Use the bits which are specific to different instruction types to determine
     *  The type of instruction.
     */
     
    uint32_t bit27 = getVal(fetched->binaryInstruction, 27, 27);
    uint32_t bit26 = getVal(fetched->binaryInstruction, 26, 26);
    uint32_t bit25 = getVal(fetched->binaryInstruction, 25, 25);
    uint32_t bit07 = getVal(fetched->binaryInstruction, 7, 7);
    uint32_t bit04 = getVal(fetched->binaryInstruction, 4, 4);

    enum instructionType instructType;
    uint32_t zero = 0;
    uint32_t one  = 1;

    /*  Algorithm for determining the instruction type from the determining bits.
     *  Check Piazza for detailed explanation.
     */
     
    if (fetched -> binaryInstruction == 0) {

        instructType = SPECIAL;
       
    } else if (bit27 == one) {
   
        instructType = BRANCH;
       
    } else if (bit26 == one) {

        instructType = SINGLE_DATA_TRANSFER;
       
    } else if ((bit25 == one) || (bit04 == zero) || (bit07 == 0)) {

        instructType = DATA_PROCESSING;
       
    } else {

        instructType = MULTIPLY;
    }

    //  For storing intermediate results.
    enum mnemonic operation;
    uint32_t      opcode;

    switch (instructType) {

        case BRANCH               :  opcode  = getVal(fetched->binaryInstruction, 31, 28);

            switch(opcode) {

                //  Using hex values since binary isnt available.
               
                case 0x0  :  operation = BEQ; break; //  0b 0000 == eq
                case 0x1  :  operation = BNE; break; //  0b 0001 != ne
                case 0xA  :  operation = BGE; break; //  0b 1010 >= ge
                case 0xB  :  operation = BLT; break; //  0b 1011 <  lt
                case 0xC  :  operation = BGT; break; //  0b 1100 >  gt
                case 0xD  :  operation = BLE; break; //  0b 1101 <= le
                case 0xE  :  operation = B;   break; //  0b 1110 T  al
               
            }   break;
           
        case MULTIPLY             :  opcode = getVal(fetched->binaryInstruction, 21, 21);

            switch(opcode) {
           
                case 0x0  :  operation = MUL; break;
                case 0x1  :  operation = MLA; break;
               
            }   break;
       
        case DATA_PROCESSING      :  opcode = getVal(fetched->binaryInstruction, 24, 21);
       
            switch(opcode) {
           
                //  Using hex values since binary isnt available.
               
                case 0x0  :  operation = AND; break; //  0b 0000
                case 0x1  :  operation = EOR; break; //  0b 0001
                case 0x2  :  operation = SUB; break; //  0b 0010
                case 0x3  :  operation = RSB; break; //  0b 0011
                case 0x4  :  operation = ADD; break; //  0b 0100
                case 0x8  :  operation = TST; break; //  0b 1000
                case 0x9  :  operation = TEQ; break; //  0b 1001
                case 0xA  :  operation = CMP; break; //  0b 1010
                case 0xC  :  operation = ORR; break; //  0b 1100
                case 0xD  :  operation = MOV; break; //  0b 1101
               
            }   break;
       
        case SINGLE_DATA_TRANSFER :  opcode = getVal(fetched->binaryInstruction, 20, 20) ;

            switch(opcode) {
           
                case 0x0  :  operation = STR; break;
                case 0x1  :  operation = LDR; break;
               
            }   break;

        case SPECIAL              :  operation = ANDEQ; break;
       
       
    }

    decoded->operation = operation;
    
    return instructType;
   
}

//  Function to decoded a previously fetched instrcution.
void decodeFetchedInstruction(void){
 
  enum instructionType instrType = mnemonicDecoder();
 
  switch(instrType){

        case DATA_PROCESSING       :  decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                      decoded->rn       = getVal(fetched->binaryInstruction, 19, 16) ;
                                      decoded->rd       = getVal(fetched->binaryInstruction, 15, 12) ;
                                      decoded->op2      = getVal(fetched->binaryInstruction, 11,  0) ;
                                      decoded->s        = getVal(fetched->binaryInstruction, 20, 20) ; 
                                      decoded->i        = getVal(fetched->binaryInstruction, 25, 25) ;
                                      break;
                                    
        case MULTIPLY              :  decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                      decoded->rd       = getVal(fetched->binaryInstruction, 19, 16) ;
                                      decoded->rn       = getVal(fetched->binaryInstruction, 15, 12) ;
                                      decoded->rs       = getVal(fetched->binaryInstruction, 11,  8) ;
                                      decoded->rm       = getVal(fetched->binaryInstruction,  3,  0) ;
                                      decoded->a        = getVal(fetched->binaryInstruction, 21, 21) ;
                                      decoded->s        = getVal(fetched->binaryInstruction, 20, 20) ;
                                      break;
                                    
        case SINGLE_DATA_TRANSFER  :  decoded->cond     = getVal(fetched->binaryInstruction, 31, 28) ;
                                      decoded->offset   = getVal(fetched->binaryInstruction, 11,  0) ;
                                      decoded->rn       = getVal(fetched->binaryInstruction, 19, 16) ;
                                      decoded->rd       = getVal(fetched->binaryInstruction, 15, 12) ;
                                      decoded->u        = getVal(fetched->binaryInstruction, 23, 23) ;
                                      decoded->l        = getVal(fetched->binaryInstruction, 20, 20) ;
                                      decoded->i        = getVal(fetched->binaryInstruction, 25, 25) ;
                                      decoded->p        = getVal(fetched->binaryInstruction, 24, 24) ;
                                      decoded->s        = 0;  //  Because lsr is called by ldr
                                      break;
                                    
        case BRANCH               :  decoded->cond      = getVal(fetched->binaryInstruction, 31, 28) ;
                                     decoded->offset    = getVal(fetched->binaryInstruction, 23,  0) ;
                                     break;
                                    
        case SPECIAL              :  break;

  }
 
  decoded->pending = T;
 
}

//  Function to print the state of the memory and registers when the program finishes.
void printFinalState(void){

  printf("Registers:\n");
 
  for (int i = 0; i<17 ; i++) {
 
    if (i!=13 && i!=14) {
    
      printf("%-4s: %10i (0x%08x)\n", ARMReg[(i)].ident, Rg(i), Rg(i) );
    }
  
  }
 
  printf("Non-zero memory:\n");
  
  for (int count = 0; count < 16384; count+=4) {
  
    if (fetchFromMem(count)!=0) {
    
      printf("0x%08x: 0x%02x%02x%02x%02x\n", (count), memPtr[count], memPtr[count + 1],
      memPtr[count + 2], memPtr[count + 3]);
    }
    
  }
}


 /*  ---------------------------------------------------------------------
 *                             EXECUTE
 *  ---------------------------------------------------------------------
 */
 
//  Function to execute decoded instructions and clear decoded instructions pending flag.
void executeDecodedInstruction(void){

    decoded->pending = F;
    enum mnemonic op = decoded->operation;
   
    switch (op){
   
        case AND :  and(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case EOR :  eor(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case SUB :  sub(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case RSB :  rsb(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case ADD :  add(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case TST :  tst(Rg(decoded->rn), getOp2());
                    break;
                    
        case TEQ :  teq(Rg(decoded->rn), getOp2());
                    break;
                    
        case CMP :  cmp(Rg(decoded->rn), getOp2());
                    break;
                    
        case ORR :  orr(Rg(decoded->rn), getOp2(), decoded->rd);
                    break;
                    
        case MOV :  mov(getOp2(), decoded->rd);
                    break;
                    
        case MLA :  ;            
        case MUL :  multiply();
                    break;     
                      
        case LDR :  ldr(decoded->offset);
                    break;  
                          
        case STR :  str(decoded->offset);
                    break;
                    
        case BEQ :  ;
        case BNE :  ;
        case BGE :  ;
        case BLT :  ;
        case BGT :  ;
        case BLE :  ;
        case B   :  branch(decoded->offset);
                    break;   
                         
        default  :  break;
  }
}
}





/*  ---------------------------------------------------------------------
 *                           MAIN FUNCTION
 *  ---------------------------------------------------------------------
 */

 int main(int argc, char **argv) {

     //  Memory initialised
     initMem();
     initdf();

/*  ---------------------------------------------------------------------
 *                            FILE LOADING
 *  ---------------------------------------------------------------------
 *
 *  Loads the binary file and stores each byte in the emulator's memory
 */
     FILE *file = fopen(argv[1], "rb");

     if (file == NULL){
     
             perror( " Error: Could not open file. \n" );
             exit(EXIT_FAILURE);
     }
    
    
     //  Calculate file size and then from the size, the number of 32-bit instructions in the file
     fseek( file, 0, SEEK_END );
     int instructionsSize = ftell(file);
     fseek( file, 0, SEEK_SET );

     //  Load the binary data into the memory array.
     fread(memPtr, 1, instructionsSize, file );

     //  File closing
     fclose(file);



/*  ---------------------------------------------------------------------
 *                      PIPELINE EXECUTION
 *  ---------------------------------------------------------------------
 *
 *  Loops infinitely until ANDEQ r0,r0,r0 (instruction 0x0, halt) is executed
 *  or the PC has reached the end of memory and there are no more fetched or decoded
 *  instructions pending.
 */
   
     //main pipeline loop.
     while ( T ) { //Loops infinitely until halt instruction is called



        if ( decoded->pending == T ) {

            if (decoded->operation == ANDEQ) { //check for special halt instruction
                break;
            }

            if ( checkCond()) { //check if conditions match CPSR

                executeDecodedInstruction();
            }
           
            decoded->pending = F;
        }

        if ( fetched->pending == T ) {
        
            decodeFetchedInstruction();
            fetched->pending = F;
           
        }

        if (PC_ <  MEM) { // PC_ is a macro defined earlier.

            fetchNextInstruction();
            fetched->pending = T;
           
        } else if (decoded->pending == F) {
       
            break;  //  Stop execution by breaking while loop.
        }


        incrementPC();
    
     }
     

     printFinalState();
    

        return EXIT_SUCCESS;

 }

