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

/* Control ROM
 * Determines control lines within the processor
 * Bits indicate each control line, individual values are mapped to
 * each microcode
 * 0 - execute
 */
const ROM_WIDTH CONTROL_ROM[64] =
{
    PERFORMCARRY | BRANCH_INSTRUCTIONREADY, // loading memory
    PERFORMCARRY, // executing instruction

    /* Load program cache 0x02 */
    LINE(SENDADDRESSPC) | LINE(SETREAD) | BRANCH_ONACKNOWLEDGE,
    LINE(ROTATEADDRESS) | STORECARRY | PUTONSTACK,
    PUTONSTACK,
    PUTONSTACK | LINE(SETBUSBIT),
    STACKROTATE,
};

/* The decoder
 * Maps 16 4-bit instructions to 6-bit microcodes
 */
const uint8_t DECODER[16] =
{
};

/* The microsequencer
 * Maps 6-bit microcodes to 6-bit microcodes
 */
const uint8_t MICROSEQUENCE[64] =
{
    0x02, // Load program cache
    0xFF, // Execute instruction

    /* Load program cache 0x02 */
    0x02, // Branches to 0x03 when address sent
    0x04,
    0x05,
    0x06,
};

/* CONTROL ROM
 * Determines whether a control line should be high or low based on the state
 */
int control_line(Afternoon1 *state, ControlLine line)
{
    ROM_WIDTH test = 1;
    
    if (CONTROL_ROM[state->MICROCODE] & (test << line))
        return 1;
    else
        return 0;
}

void compute_microcode(Afternoon1 *state, Afternoon1 *next)
{
    int conditional;

    // EXECUTE - load microinstruction from decoder
    if (control_line(state, EXECUTE))
        next->MICROCODE = DECODER[state->program_cache[0]];
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
