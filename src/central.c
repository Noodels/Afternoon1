#include <stdlib.h>

#include "afternoon1.h"
#include "stack.h"
#include "memory.h"
#include "cache.h"
#include "blu.h"

int afternoon1_setup(Afternoon1 *new, int stacksize, int cachesize)
{
    new->MICROCODE = 0;

    new->program_cache[0] = 0;
    new->program_cache[1] = 0;
    new->program_cache[2] = 0;
    new->program_cache[3] = 0;

    new->carry_register = 0;

    new->stackptr = calloc(1, stacksize);
    new->stack_size = stacksize;
    
    new->main_data_input = 0;
    
    new->current_address = 0;
    new->memory_request = 0;

    new->cache = calloc(2, cachesize); // 1 - 16
    new->cache_size = cachesize;

    new->program_counter = 0;

    new->DATAIN = 0;
    new->DATAOUT = 0;

    if ((new->cache == NULL) || (new->stackptr == NULL))
        return -1;
    else
        return 0;
}

void afternoon1_free(Afternoon1 *scrap)
{
    free(scrap->cache);
    free(scrap->stackptr);
}

void afternoon1_databus(Afternoon1 *state)
{
    state->main_data_input = 0
        | stack_result(state)
        | cache_result(state)
        | pc_result(state)
        | codecache_result(state)
        | blu_result(state)
        ;
}

void afternoon1_step(Afternoon1 *state, Afternoon1 *next)
{
    // Assertion: Main data bus is set up and ready
    compute_blu(state, next);
    compute_codecache(state, next);
    compute_programcounter(state, next);
    compute_cache(state, next);
    compute_memaccess(state, next);
    compute_microcode(state, next);
    compute_stack(state, next);
}
