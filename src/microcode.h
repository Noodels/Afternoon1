#define ROM_WIDTH int16_t

typedef enum {
    // Next microcode loaded from decoder
    EXECUTE=0,

    /* Branching lines, work as follows:
     * 000 - no action, leave microcode as-is
     * 001 - check instruction readiness
     * 010 - check memory readiness
     * 011 - check for carries in carry register
     * 100 - check for zero on top of stack */
    COND0,
    COND1,
    COND2,

    /* Stack control lines
     * W - whether each item on the stack should write
     * RD - direction to read from if writing, 0=above, 1=down
     * RD2 and W3 apply to all the registers below themselves */
    W0, RD0,
    W1, RD1,
    W2, RD2,
    W3,
} ControlLine;

int control_line(Afternoon1*, ControlLine);
void compute_microcode(Afternoon1*, Afternoon1*);
