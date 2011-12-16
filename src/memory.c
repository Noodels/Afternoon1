#include "afternoon1.h"
#include "microcode.h"

void compute_programcounter(Afternoon1 *state, Afternoon1 *next)
{
    if (control_line(state, ROTATEADDRESS) || control_line(state, PUTADDRESS))
        next->program_counter = (state->program_counter << 4)
            | (state->stackptr[0]);
}

void compute_cache(Afternoon1 *state, Afternoon1 *next)
{
    /* Operate the FIFO */
    if (control_line(state, FIFO) &&
            state->stackptr[1] < state->cache_size) // check cache range
        next->cache[state->stackptr[1]] =
            (state->cache[state->stackptr[1]] << 4) // all shift down
            | state->stackptr[0]; // to make room for item on top of stack
        // operates on cache lookup

    /* Operate memory input */
    if (control_line(state, STORECACHE))
    {
        if (state->stackptr[1] < state->cache_size) // check cache range
            next->cache[state->stackptr[1]] = state->DATAIN;
        next->memory_request |= 3; // acknowledge
    }
    /* Operate memory output */
    next->DATAOUT = state->cache[(state->cache_size-1)];
}

void compute_memaccess(Afternoon1 *state, Afternoon1 *next)
{
    uint8_t t = (control_line(state, SETREAD) << 1)
        | control_line(state, SETWRITE);

    next->memory_request |= t;
    if (t)
    {
        if (control_line(state, SENDADDRESSPC))
            next->current_address = state->program_counter;
        else
        {
            if (state->stackptr[1] < state->cache_size)
                next->current_address = state->cache[state->stackptr[1]];
                // current address set to value of cache lookup
            else
                next->current_address = 0; // cache out of range
        }
    }
}
