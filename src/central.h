#include "afternoon1.h"

/* Initialises to default values */
int afternoon1_setup(Afternoon1*);
void afternoon1_free(Afternoon1*); // deinitialise

/* Wrapper for all the afternoon1 computation and bus control
 * Takes a source and a destination state of processor */
void afternoon1_step(Afternoon1*, Afternoon1*);
