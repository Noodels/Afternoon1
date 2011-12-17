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

    free(new->stackptr);
    new->stackptr = calloc(1, stacksize);
    new->stack_size = stacksize;
      
    new->current_address = 0;
    new->memory_request = 0;

    free(new->cache);
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

int afternoon1_ghostcopy(Afternoon1 *src, Afternoon1 *dst)
{
    free(dst->cache);
    free(dst->stackptr);

    dst->stack_size = src->cache_size;
    dst->stackptr = calloc(1, src->stack_size);

    dst->cache_size = src->cache_size;
    dst->cache = calloc(2, src->cache_size);

    if ((dst->cache == NULL) || (dst->stackptr == NULL))
        return -1;
    else
        return 0;
}

void afternoon1_free(Afternoon1 *scrap)
{
    free(scrap->cache);
    free(scrap->stackptr);
}

void afternoon1_step(Afternoon1 *state, Afternoon1 *next)
{ 
    compute_blu(state, next);
    compute_codecache(state, next);
    compute_programcounter(state, next);
    compute_cache(state, next);
    compute_memaccess(state, next);
    compute_microcode(state, next);
    compute_stack(state, next);
}

void afternoon1_cycles(Afternoon1 *source, Afternoon1 *inter,
        Afternoon1 *dest, int n)
{
    Afternoon1 *t;
    int i;
    
    if (n % 2)
    {   // if odd no. of cycles, ensure dest is written to first
        // this will guarantee that dest is written to last
        // 1: dest
        // 3: dest, inter, dest
        // 5: dest, inter, dest, inter, dest
        t = inter;
        inter = dest;
        dest = t;
    }
        // if even no. of cycles, inter is written to first
        // 2: inter, dest
        // 4: inter, dest, inter, dest

    // initial source to inter step, leaving (n-1) cycles to do
    afternoon1_step(source, inter);

    for (i=1; i<n; i++)
    {
        afternoon1_step(inter, dest);

        // And swap around
        t = inter;
        inter = dest;
        dest = t;
    }
}
