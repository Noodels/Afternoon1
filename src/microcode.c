#include "afternoon1.h"
#include "microcode.h"

#define LINE(n) (1 << n)

// Carry operations
#define PERFORMCARRY LINE(BLUCARRY)
#define STORECARRY   LINE(BLUCARRY)|LINE(BLU1)
#define RESTORECARRY LINE(BLUCARRY)|LINE(BLU0)

// Conditional operations
#define BRANCH_INSTRUCTIONREADY LINE(COND0)
#define BRANCH_MEMORYREADY      LINE(COND1)
#define BRANCH_CARRYREGISTER    LINE(COND0)|LINE(COND1)
#define BRANCH_ONZERO           LINE(COND2)
#define BRANCH_ONACKNOWLEDGE    LINE(COND2)|LINE(COND0)

// Operations on the stack
#define PUTONSTACK  LINE(W0)|LINE(W1)|LINE(W2)|LINE(W3)
#define STACKROTATE LINE(LOOPBACK1)|LINE(W0)|LINE(W1)|LINE(W2)
#define REPLACE     LINE(W0)
#define DROP2       LINE(W1)|LINE(RD1)|LINE(W2)|LINE(RD2)|LINE(W3)

// BLU Operations
#define RELEASECARRIES  LINE(BLU0)
#define LEFTSHIFT       LINE(BLU1)
#define RIGHTSHIFT      LINE(BLU1)|LINE(BLU0)
#define BLU_AND         LINE(BLU2)
#define BLU_XOR         LINE(BLU2)|LINE(BLU0)
#define BLU_XOR_CARRY   LINE(BLU2)|LINE(BLU1)
#define BLU_OR          LINE(BLU2)|LINE(BLU1)|LINE(BLU0)

/* Control ROM
 * Determines control lines within the processor
 * Bits indicate each control line, individual values are mapped to
 * each microcode
 * 0 - execute
 */
const ROM_WIDTH CONTROL_ROM[64] =
{
    PERFORMCARRY, // loading memory
    PERFORMCARRY, // executing instruction

    /* Load program cache 0x02 */
    LINE(SENDADDRESSPC) | LINE(SETREAD) | BRANCH_ONACKNOWLEDGE,
    LINE(ROTATEADDRESS) | STORECARRY | PUTONSTACK,
    PUTONSTACK,
    PUTONSTACK | LINE(SETBUSBIT),
    STACKROTATE,
    BLU_XOR_CARRY | REPLACE | DROP2 | BRANCH_CARRYREGISTER, // 7

    /* PC Add #1 */
    /* 0x08 */  LINE(ROTATEADDRESS) | PERFORMCARRY | REPLACE, // end addition
    /* 0x09 */  RELEASECARRIES | PUTONSTACK, // jumps to 0x07

    BLU_XOR_CARRY | REPLACE | DROP2 | BRANCH_CARRYREGISTER, // A
    // Skip over this instruction, it is revisited after addition
    LINE(ROTATEADDRESS) | BRANCH_MEMORYREADY | REPLACE,
    /* PC Add #2 */
    /* 0x0C */  LINE(ROTATEADDRESS) | RESTORECARRY, // end add
    /* 0x0D */  RELEASECARRIES | PUTONSTACK, // jumps to 0x0A

    /* 0x0E, complete address rotation */
    // memory ready, store code
    LINE(STORECODE),
    BRANCH_MEMORYREADY, // memory not ready condition, loops

    /* 0x10, execute instruction */
    LINE(EXECUTE) | LINE(CYCLECODECACHE),

    /* 0x11 NOP instruction */
    BRANCH_INSTRUCTIONREADY,

    /* 0x12 FIFO instruction */
    BRANCH_INSTRUCTIONREADY | LINE(FIFO) | LINE(REPLACE),

    /* 0x13 free */

    /* 0x14 READ instruction */
    /* 0x14 */  LINE(SETREAD) | BRANCH_ONACKNOWLEDGE,
    /* 0x15 */  BRANCH_MEMORYREADY,

    /* 0x16 */  LINE(STORECACHE) | BRANCH_INSTRUCTIONREADY, //end
    /* 0x17 */  BRANCH_MEMORYREADY, // trap

    /* 0x18 WRITE instruction */
    /* 0x18 */  LINE(SETWRITE) | BRANCH_ONACKNOWLEDGE,
    /* 0x19 */  BRANCH_MEMORYREADY,

    /* 0x1A */  BRANCH_INSTRUCTIONREADY,
    /* 0x1B */  BRANCH_MEMORYREADY,

    /* 0x1C LDI instruction */
    LINE(CYCLECODECACHE) | LINE(PUTIMMEDIATE) | PUTONSTACK,
    BRANCH_INSTRUCTIONREADY,
};

/* The decoder
 * Maps 16 4-bit instructions to 6-bit microcodes
 */
const uint8_t DECODER[16] =
{
    0x11, // 0 NOP instruction
    0x12, // 1 FIFO instruction
    0x14, // 2 READ instruction
    0x18, // 3 WRITE instruction
};

/* The microsequencer
 * Maps 6-bit microcodes to 6-bit microcodes
 */
const uint8_t MICROSEQUENCE[64] =
{
    /*0x00*/    0x02, // Load program cache
    /*0x01*/    0x10, // Execute instruction

        /* Load program cache 0x02 */
    /*0x02*/    0x02, // Branches to 0x03 when address sent
    /*0x03*/    0x04,
    /*0x04*/    0x05,
    /*0x05*/    0x06,
    /*0x06*/    0x07,
    /*0x07*/    0x08, // Branches to 0x09 when carries
    /*0x08*/    0x0A, // End of addition, go to next instruction
    /*0x09*/    0x07, // Returns to XOR addition
    /*0x0A*/    0x0C, // Branches to 0x0D when carries
    /*0x0B*/    0x0E, // Branches to 0x0E when ready
    /*0x0C*/    0x0B, // End of addition, go to next instruction
    /*0x0D*/    0x0A, // Returns to XOR addition
    /*0x0E*/    0x10,
    /*0x0F*/    0x0E, // Branches to 0x0E when ready

    /*0x10*/    0xFF, // always execute here, M.I. ignored

    /* The NOP Instruction */
    /*0x11*/    0x00, // end instruction

    /* The FIFO Instruction */
    /*0x12*/    0x00, // end instuction

    /*0x13*/    0xFF, // unused

    /* The READ Instruction */
    /*0x14*/    0x14, // to 0x15 when acknowledged
    /*0x15*/    0x16, // branches to 0x17 until ready
    /*0x16*/    0x00, // end instruction
    /*0x17*/    0x16, // branches to 0x17 until ready

    /* The WRITE Instruction */
    /*0x18*/    0x18, // branches to 0x19 when acknowledged
    /*0x19*/    0x1A, // branches to 0x1B until ready

    /*0x1A*/    0x00, // end instruction
    /*0x1B*/    0x1A, // branches to 0x1B until read

    /* The LDI Instruction */
    /*0x1C*/    0x1D,
    /*0x1D*/    0x00,
};

/* CONTROL ROM
 * Determines whether a control line should be high or low based on the state
 */
int control_line(Afternoon1 *state, ControlLine line)
{   
    if (CONTROL_ROM[state->MICROCODE] & LINE(line)) 
        return 1;
    else
        return 0;
}

void compute_microcode(Afternoon1 *state, Afternoon1 *next)
{
    int conditional;

    // EXECUTE - load microinstruction from decoder
    if (control_line(state, EXECUTE))
        next->MICROCODE = DECODER[state->program_cache[0] & 0xF];
    else
    {
        next->MICROCODE = MICROSEQUENCE[state->MICROCODE];
        conditional = control_line(state, COND2) << 2
            | control_line(state, COND1) << 1
            | control_line(state, COND0);
        switch (conditional)
        {
            case 1:
                // Check for instruction readiness
                // Branches to XXXXX1 if ready
                next->MICROCODE &= 0xFE;
                if (state->program_cache[0] & 0x10)
                    next->MICROCODE |= 0x01;
                break;
            case 2:
                // Check for memory readiness
                // Branches to XXXXX0 if ready
                next->MICROCODE &= 0xFE;
                if (state->memory_request)
                    next->MICROCODE |= 0x01;
                break;
            case 3:
                // Check for carries
                // Branches to XXXXX1 if carries available
                next->MICROCODE &= 0xFE;
                // Check C(-1), C(0), C(1) and C(2)
                if (state->carry_register & 0x0F)
                    next->MICROCODE |= 0x01;
                break;
            case 4:
                // Check for zero in user-space branch
                // Branches to XXXXX1 if zero
                next->MICROCODE &= 0xFE;
                if (state->stackptr[0] == 0)
                    next->MICROCODE |= 0x01;
                break;
            case 5:
                // Sendaddr was triggered, ensure memory ready
                // Branches to XXXXX1 if ready
                next->MICROCODE |= 0x01;
                if ((state->memory_request & 0x03) == 0x03)
                    next->MICROCODE &= 0xFE;
                break;
            otherwise:
                // Assume value is 0, illegal values could be printed
                break;
        }
    }

    // Assertion: next->MICROCODE is now set and correct
}
