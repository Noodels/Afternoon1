#include "afternoon1."

#include "blu.h"
#include "cache.h"
#include "memory.h"
#include "microcode.h"
#include "stack.h"

uint8_t bus_result(Afternoon1 *state)
{
    return ( 0
            | blu_result(state)
            | codecache_result(state)
            | pc_result(state)
            | cache_result(state)
            | stack_result(state)
            | (uint8_t)(control_line(state, SETBUSBIT));A
           );
}
