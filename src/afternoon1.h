#include <stdint.h>

/* 
 * The struct for an Afternoon1 processor, stores all local registers
 */
typedef struct
{
    uint8_t MICROCODE; // Microcode register (6 bits)
    uint8_t program_cache[4]; // Program cache, uses 5 bits per piece
    uint8_t memory_request; // uses 2 bits
    uint8_t carry_register; // uses 5 bits, C(-1) to C(3)
                            // C(3) does not cause looping

    uint8_t *stackptr; // stack pointer, 4 bits per piece
    unsigned int stack_size; // size of stack, minimum 4

    /* BUSES - Need to be updated before computation occurs */
    uint8_t main_data_input; // 4 bits used
    // uint8_t main_databus; // Not used, use stackptr[0]
} Afternoon1;
