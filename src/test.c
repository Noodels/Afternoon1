#include <stdio.h>
#include "central.h"
#include "microcode.h"

int test_firstmicrocodes(Afternoon1 *state, Afternoon1 *next)
{
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

    // Test passed
    printf("[    ] End of section: first microcodes\n");
    return 0;
}

int test_allcases(Afternoon1 *p1, Afternoon1 *p2)
{
    if (test_firstmicrocodes(p1, p2))
        return 1;
    afternoon1_free(p1);
    afternoon1_free(p2);

    return 0;
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
