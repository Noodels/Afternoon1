#define ROM_WIDTH int32_t

typedef enum {
    // Next microcode loaded from decoder
    EXECUTE=0,

    /* Branching lines, work as follows:
     * 000 - no action, leave microcode as-is
     * 001 - check instruction readiness
     * 010 - check memory readiness
     * 011 - check for carries in carry register
     * 100 - check for zero on top of stack
     * */
    COND0,
    COND1,
    COND2,
} ControlLine;
