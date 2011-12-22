#include <stdio.h>
#include "central.h"
#include "microcode.h"

int test_firstmicrocodes(Afternoon1 *state, Afternoon1 *next)
{
    int i, j;

    afternoon1_setup(state, 32, 8);
    afternoon1_setup(next, 32, 8);

    afternoon1_step(state, next);

    if (next->MICROCODE == 0x02)
        printf("[PASS] Microcode set to next instruction without inputs\n");
    else
    {
        printf("[FAIL] Microcode did not load next instruction correctly\n");
        return 1;
    }

    afternoon1_step(next, state);
    
    if (state->MICROCODE == 0x03)
        printf("[PASS] Microcode branches through a sendaddress\n");
    else
    {
        printf("[FAIL] Microcode does not branch through sendaddress\n");
        return 1;
    }

    afternoon1_step(state, next);
    afternoon1_step(next, state);
    afternoon1_step(state, next);

    if (next->stackptr[0] == 1
            && next->stackptr[1] == 0
            && next->stackptr[2] == 0)
        printf("[PASS] Basic stack insertion\n");
    else
    {
        printf("[FAIL] Basic stack insertion: %d %d %d\n",
                next->stackptr[0],
                next->stackptr[1],
                next->stackptr[2]);
        return 1;
    }

    afternoon1_step(next, state);

    if (state->stackptr[0] == 0
            && state->stackptr[1] == 1
            && state->stackptr[2] == 0)
        printf("[PASS] Stack rotation\n");
    else
    {
        printf(
"[FAIL] Stack rotation\n[    ] Intended: 0, 1, 0\n[    ] Got     : %d, %d, %d\n",
                state->stackptr[0],
                state->stackptr[1],
                state->stackptr[2]);
        return 1;
    }

    afternoon1_step(state, next);
    if (next->MICROCODE == 0x08)
        printf("[PASS] Add 1 without carry\n");
    else
    {
        printf("[FAIL] Branched to microcode 0x%x for adding without carry\n",
                next->MICROCODE);
        return 1;
    }

    afternoon1_step(next, state);
    if (state->program_counter == (0x01 << 12))
        printf("[PASS] PC Rotating correctly (#1)\n");
    else
    {
        printf("[FAIL] PC rotated to 0x%.4x (#1)\n", state->program_counter);
        return 1;
    }

    afternoon1_step(state, next);
    afternoon1_step(next, state);
    afternoon1_step(state, next);
    afternoon1_step(next, state);
    afternoon1_step(state, next);

    if (next->MICROCODE == 0x0F)
        printf("[PASS] Pausing for memory input\n");
    else
    {
        printf("[FAIL] Not attaining memory wait loop, ending at 0x%.2x\n",
                next->MICROCODE);
        return 1;
    }

    next->memory_request = 0; // ready

    afternoon1_step(next, state);
    if (state->MICROCODE == 0x0E)
        printf("[PASS] Branched out of wait loop\n");
    else
    {
        printf("[FAIL] Incorrect wait loop exit: 0x%.2x\n", state->MICROCODE);
        return 1;
    }
    
    state->DATAIN = 0xDA00; // nop, nop, ldi, (D)
    afternoon1_step(state, next);
    afternoon1_step(next, state);

    if (state->MICROCODE == 0x11)
        printf("[PASS] Executing a NOP\n");
    else
    {
        printf("[FAIL] Executing wrong instruction: 0x%.2x\n",
                state->MICROCODE);
        return 1;
    }

    // Test passed
    printf("[    ] End of section: first microcodes\n");
    return 0;
}

int test_allcases(Afternoon1 *p1, Afternoon1 *p2)
{
    int r;
    if (test_firstmicrocodes(p1, p2))
        r = 1;
    afternoon1_free(p1);
    afternoon1_free(p2);

    return r;
}

int main(void)
{
    Afternoon1 p1, p2;
    int r;

    r = test_allcases(&p1, &p2);

    if (r)
    {
        printf("Testing rig [FAILED] on test number: %d\n", r);
        return 1;
    }
    else
    {
        printf("Testing rig [PASSED]\n");
        return 0;
    }
}
