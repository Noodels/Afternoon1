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
} Afternoon1;
