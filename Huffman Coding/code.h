# ifndef _CODE_H
# define _CODE_H

# include <stdint.h>
# include <stdbool.h>

typedef struct code
{
    uint8_t bits[32]; // Holds 256 bits
    uint32_t l; // Holds length of code
} code;

// Create a new code
static inline code newCode()
{ 
    code t;
    // Set all of the bits to 0
    for (int i = 0; i < 32; i += 1)
    {
        t.bits[i] = 0;
    }
    t.l = 0;
    return t;
}

// Change the next bit to a given value
static inline bool pushCode(code *c, uint32_t k)
{
    // Push fails if there's more than 256 bits already
    if (c->l > 256)
    {
        return false;
    }
    // Set next bit to 0 if k is 0
    else if (k == 0)
    {
        c->bits[c->l / 8] &= ~(0x1 << (c->l % 8));
        c->l += 1;
    }
    // Else set the next bit to 1
    else
    {
        c->bits[c->l / 8] |=  (0x1 << (c->l % 8));
        c->l += 1;
    }
    return true;
}

// Get rid of the most recently added bit
static inline bool popCode(code *c, uint32_t *k)
{
    // Pop fails if there were no bits
    if (c->l == 0)
    {
        return false;
    }
    else
    {
        c->l -= 1; // There's now one less bit
        // Set k to the most recently added bit
        *k = ((0x1 << (c->l % 8)) & c->bits[c->l / 8]) >> (c->l % 8);
        return true;
    }
}

// Check if the code as empty
static inline bool emptyCode(code *c) 
{
    return c->l == 0; 
}

// Check if the code as full
static inline bool fullCode (code *c) 
{
    return c->l == 256;
}

# endif
