// bv.h - Bit Vector interface

# ifndef _BVector
# define _BVector
# include <stdint.h>
# include <stdlib.h>

# include "code.h"

typedef struct bitV {
	uint8_t *vector;
	int32_t position;
	uint32_t length;
} bitV;

// Create a new bit vector
bitV *newVec(uint32_t);

// Delete a bit vector
void delVec(bitV *);

// Set the next bit to a desired value
int32_t setNextBit(bitV *, uint32_t);

// Set all of the bits in the bit vector to 0
void cleanVec(bitV *);

// Set a desired bit to 1
void setBit(bitV *, uint32_t);

// Set a desired bit to 0
void clrBit(bitV *, uint32_t);

// Returns the value of a desired bit
uint8_t valBit(bitV *, uint32_t);

// Returns the length of a bit vector
uint32_t lenVec(bitV *);

# endif
