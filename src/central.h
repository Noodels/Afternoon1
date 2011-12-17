#include "afternoon1.h"

/* Initialises to default values
 * Requires a target, a stacksize, and a cache size, in that order*/
int afternoon1_setup(Afternoon1*, int, int);
void afternoon1_free(Afternoon1*); // deinitialise

/* Initialises the second value in such a way that it may store results from
 * the source argument */
int afternoon1_ghostcopy(Afternoon1*, Afternoon1*);

/* Wrapper for all the afternoon1 computation and bus control
 * Takes a source and a destination state of processor */
void afternoon1_step(Afternoon1*, Afternoon1*);

/* Wrapper for stepping between states, performs a given number
 * of cycles, preserving the first argument after one initial
 * read, mangling the second argument, and writing to the last
 * argument */
void afternoon1_cycles(Afternoon1*, Afternoon1*, Afternoon1*, int);
