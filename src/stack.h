/* Compute Stack
 * Reads read and write lines for the stack and shifts all data accordingly
 * Note: Uses main_data_input of first operand, the computation of which
 *       MUST precede the computation of the stack */
void compute_stack(Afternoon1*, Afternoon1*);

/* Stack Result
 * The value of whatever the stack is driving onto the data bus */
uint8_t stack_result(Afternoon1*);
