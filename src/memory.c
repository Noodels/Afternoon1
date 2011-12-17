#include "afternoon1.h"
#include "microcode.h"

#include <string.h>

void compute_programcounter(Afternoon1 *state, Afternoon1 *next)
{
    if (control_line(state, ROTATEADDRESS) || control_line(state, PUTADDRESS))
        next->program_counter = (state->program_counter >> 4)
            | ((uint16_t)state->stackptr[0] << 12);
    else
        next->program_counter = state->program_counter;
}

uint8_t pc_result(Afternoon1 *state)
{
    if (control_line(state, ROTATEADDRESS))
        return (state->program_counter & 0x000F);
    else
        return 0;
}

void compute_cache(Afternoon1 *state, Afternoon1 *next)
{
    /* Preserve states, later operations overwrite for changes */
    memcpy(next->cache, state->cache, sizeof(uint16_t) * next->cache_size);

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
        //next->memory_request |= 3; // acknowledge // moved
    }
    /* Operate memory output */
    next->DATAOUT = state->cache[(state->cache_size-1)];
}

uint8_t cache_result(Afternoon1 *state)
{
    if (control_line(state, FIFO) && (state->stackptr[1] < state->cache_size))
        return (state->cache[state->stackptr[1]]) >> 12;
    else
        return 0;
}

void compute_memaccess(Afternoon1 *state, Afternoon1 *next)
{
    uint8_t t = (control_line(state, SETREAD) << 1)
        | control_line(state, SETWRITE);

    if ((control_line(state, STORECACHE)) || control_line(state, STORECODE))
        t |= 0x03; // ACKNOWLEDGE memory read
    next->memory_request = state->memory_request | t;
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
