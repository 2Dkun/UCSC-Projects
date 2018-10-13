# include "stack.h"

// Creates a new stack
stack *newStack()
{
    stack *s = (stack *) malloc(sizeof(stack));
    s->size = 1;
    s->top = 0;
    s->entries = (item *) calloc(s->size, sizeof(item));
    return s;
}

// Deletes a stack
void delStack(stack *s)
{
    free(s->entries);
    free(s);
}

// Returns the top item
item pop(stack *s)
{
    if (!emptyS(s))
    {
        s->top -= 1;
        return s->entries[s->top];
    }
    return 0;
}

// Adds an item to the top
void push(stack *s, item i)
{
    if (fullS(s))
    {
        s->size *= 2;
        s->entries = (item *) realloc(s->entries, s->size * sizeof(item));
    }

    s->entries[s->top] = i;
    s->top += 1;
    return;
}

// Informs if stack is empty
bool emptyS(stack *s)
{
    return s->top == 0;
}

// Informs if stack is full
bool fullS(stack *s)
{
    return s->top == s->size;
}
