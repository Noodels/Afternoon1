#include "afternoon1.h"
#include "microcode.h"

/* BLU Functions:
 * 000 - no action
 * 001 - releasecarries
 * 010 - left shift
 * 011 - right shift
 * 100 - and
 * 101 - xor
 * 110 - xor observing carries
 * 111 - or (not used)
 */

uint8_t blu_getfunction(Afternoon1 *state)
{
    return ((control_line(state, BLU2) << 2)
            | (control_line(state, BLU1) << 1)
            | (control_line(state, BLU0)));
}

void compute_blu(Afternoon1 *state, Afternoon1 *next)
{
    // The BLU only stores carries, everything else is driven straight to the
    // databus with the blu_result function
    uint8_t input = (state->stackptr[0] & 0x0F); // assert 4 bits

    // Note that this function is only concerned with next->carry_register
    switch (blu_getfunction(state))
    {
        case 2:
            // left shift
            next->carry_register = (input << 1) & 0x10;
            break;
        case 3:
            // right shift
            next->carry_register = (input << 4) & 0x10;
            break;
        case 6:
            // xor and observe carries
            next->carry_register |= (input ^ 0x0F) << 1;
            break;
        otherwise:
            break;
    }

    // Assertion: next->carry_register is now set and correct
}

uint8_t blu_result(Afternoon1 *state)
{
    switch (blu_getfunction(state))
    {
        case 0: // nop
            return 0;
            break;
        case 1: // releasecarries
            return (state->carry_register & 0x0F);
            break;
        case 2: // left shift
            return ((state->stackptr[0] << 1) | (state->carry_register & 1));
            break;
        case 3: // right shift
            return ((state->stackptr[0] >> 1)
                    | ((state->carry_register << 3) & 0x08));
            break;
        case 4: // and
            return ((state->stackptr[0]) & (state->stackptr[1]));
            break;
        case 5: // xor
            // see below
        case 6: // xor observing carries
            return ((state->stackptr[0]) ^ (state->stackptr[1]));
            break;
        case 7: // or, unless disabled
            return ((state->stackptr[0]) | (state->stackptr[1]));
            break;
        otherwise: // should never happen
            return 0;
    }
}
