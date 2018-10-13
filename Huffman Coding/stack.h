# ifndef _STACK_H
# define _STACK_H
# include <stdint.h>
# include <stdbool.h>

# include "huffman.h"
# include <stdlib.h>

typedef treeNode *item;

typedef struct stack
{
    uint32_t size; // Holds size
    uint32_t top; // Keeps track of the top of the stack
    item *entries; // Array to hold items
} stack;

stack *newStack(); // Creates a new stack
void delStack(stack *); // Deletes a stack

item pop(stack *); // Returns the top item
void push(stack *, item); // Adds an item to the top

bool emptyS(stack *); // Informs if stack is empty
bool fullS(stack *); // Informs if stack is full

# endif
