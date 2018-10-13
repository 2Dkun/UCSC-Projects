# include "bv.h"

// Create a new bit vector of desired length
bitV *newVec(uint32_t u)
{
    bitV *v = (bitV *) malloc(sizeof(bitV));
    v->vector = (uint8_t *) calloc(u/8+1, sizeof(uint8_t));
    v->position = 0;
    v->length = u;
    return v;
}

// Delete a bit vector
void delVec(bitV *v)
{
    free(v->vector);
    free(v);
    return;
}

// Set the next bit to a desired value
int32_t setNextBit(bitV *v, uint32_t u)
{
    if (u == 0) // Clear bit if value is 0
    {
        clrBit(v, v->position);
    }
    else // Set bit if the value is 1
    {
        setBit(v, v->position);
    }
    
    // Move the position of the vector forward by 1
    v->position += 1;
    // Loop back to the first position once you hit 8
    if (v->position >= 8)
    {
        v->position = 0;
        return 1;
    }
    return 0;
}

// Set all bits in the bit vector to 0
void cleanVec(bitV *v)
{
    for (uint32_t i = 0; i <= lenVec(v)/8+1; i += 1)
    {
        v->vector[i] &= 0x0;
    }
    return;
}

// Set a desired bit
void setBit(bitV *v, uint32_t u)
{
    v->vector[u/8] |= 0x1 << u%8;
    return;
}

// Clear a desired bit
void clrBit(bitV *v, uint32_t u)
{
    v->vector[u/8] &= ~(0x1 << u%8);
    return;
}

// Return the value of a desired bit
uint8_t valBit(bitV *v, uint32_t u)
{
    return (v->vector[u/8] & (0x1 << u%8)) >> u%8;
}

// Return the length of the bit vector
uint32_t lenVec(bitV *v)
{
    return v->length;
}
