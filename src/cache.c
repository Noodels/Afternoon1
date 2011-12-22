#include "afternoon1.h"
#include "microcode.h"

void compute_codecache(Afternoon1 *state, Afternoon1 *next)
{
    int i;

    if (control_line(state, CYCLECODECACHE))
    {
        for (i=0; i<3; i++)
            next->program_cache[i] = state->program_cache[i+1];
        next->program_cache[3] = 0;
    }
    else if (control_line(state, STORECODE))
    {
        for (i=0; i<4; i++)
            next->program_cache[i] =
                0x10 | (0xF & (state->DATAIN >> (i*4)));
    }
    else
    {
        // no change, preserve state
        for (i=0; i<3; i++)
            next->program_cache[i] = state->program_cache[i];
    }

    // memory_request is a variable not in the jurisdiction of this scope
    // it should be set up all in one place, so this has been moved to
    // memory.c, in its own function, also, ACK code should be 0x03 anyway
    /*if (control_line(state, STORECODE))
    { 
        for (i=0; i<4; i++)
            next->program_cache[i] = ((uint8_t)(state->DATAIN >> (i*4)) & 0xF) | 0x10;
        next->memory_request |= 0x04; // ACKnowledge
    }*/
}

uint8_t codecache_result(Afternoon1 *state)
{
    if (control_line(state, PUTIMMEDIATE))
        return (state->program_cache[0]);
    else
        return 0;
}
