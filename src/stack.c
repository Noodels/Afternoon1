#include "afternoon1.h"
#include "microcode.h"
#include "stack.h"

void compute_stack(Afternoon1 *state, Afternoon1 *next)
{
    unsigned int i;

    if (control_line(state, W0))
    {
        if (control_line(state, RD0))
            next->stackptr[0] = state->stackptr[1]; // from "below"
        else
            next->stackptr[0] = state->main_data_input; // from "above"
    }

    if (control_line(state, W1))
    {
        if (control_line(state, RD1))
            next->stackptr[1] = state->stackptr[2]; // from "below"
        else
            next->stackptr[1] = state->stackptr[0]; // from "above"
    }

    if (control_line(state, W2))
    {
        if (control_line(state, RD2))
            next->stackptr[2] = state->stackptr[3]; // from "below"
        else
            next->stackptr[2] = state->stackptr[1]; // from "above"
    }

    // Uses RD2
    if (control_line(state, W3))
    {
        if (control_line(state, RD2))
        { // from "below"
            for (i=4; i<(state->stack_size); i++)
                next->stackptr[i-1] = state->stackptr[i];
            next->stackptr[i-1] = 0;
        }
        else
        { // from "above"
            for (i=3; i<(state->stack_size); i++)
                next->stackptr[i] = state->stackptr[i-1];
        }
    }
}
