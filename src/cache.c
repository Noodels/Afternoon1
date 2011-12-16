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

    if (control_line(state, STORECODE))
    { 
        for (i=0; i<4; i++)
            next->program_cache[i] = ((uint8_t)(state->DATAIN >> (i*4)) & 0xF) | 0x10;
        next->memory_request |= 0x04; // ACKnowledge
    }
}

uint8_t codecache_result(Afternoon1 *state)
{
    if (control_line(state, PUTIMMEDIATE))
        return (state->program_cache[0]);
    else
        return 0;
}
