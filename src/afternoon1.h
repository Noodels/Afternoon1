#include <stdint.h>

/* 
 * The struct for an Afternoon1 processor, stores all local registers
 */
typedef struct
{
    uint8_t MICROCODE; // Microcode register (6 bits)
    uint8_t program_cache[4]; // Program cache, uses 5 bits per piece
    uint8_t carry_register; // uses 6 bits, C(-1) to C(3) + C(store)
                            // C(3) does not cause looping

    uint8_t *stackptr; // stack pointer, 4 bits per piece
    unsigned int stack_size; // size of stack, minimum 4

    /* BUSES - Need to be updated before computation occurs */
    uint8_t main_data_input; // 4 bits used
    // uint8_t main_databus; // Not used, use stackptr[0]
    
    /* Memory IO */
    uint16_t current_address;
    uint8_t memory_request; // uses 2 bits
                            // 00 - no operations
                            // 01 - write
                            // 10 - read
                            // 11 - acknowledge

    uint16_t *cache;
    unsigned int cache_size;

    uint16_t program_counter;

    uint16_t DATAIN, DATAOUT; // input and output to the processor
} Afternoon1;
