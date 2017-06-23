# ifndef _HUFFMAN_H
# define _HUFFMAN_H
# include <stdint.h>
# include <stdbool.h>

# ifndef NIL
# define NIL (void *) 0
# endif

# include "code.h"
# include <stdlib.h>

typedef struct DAH treeNode;

struct DAH
{
	uint8_t symbol;
	uint64_t count;
	bool leaf;
	treeNode *left, *right;
};

// New node, with symbols, leaf or not, a count associated with it
treeNode *newNode(uint8_t s, bool l, uint64_t c);

// Dump a Huffman tree onto a file
void dumpTree(treeNode *t, int32_t file);

// Build a tree from the saved tree
treeNode *loadTree(uint8_t savedTree[], uint16_t treeBytes);

// Step through a tree following the code
int32_t stepTree(treeNode *root, treeNode **t, uint32_t code);

// Parse a Huffman tree to build codes
void buildCode(treeNode *t, code s, code table[256]);

// Delete a tree
void *delTree(treeNode *t);

// Delete a node
static inline void delNode(treeNode *h)
{
	free(h);
	return;
}

// Compares the count of each tree
static inline int8_t compare(treeNode *l, treeNode *r)
{
	return l->count - r->count; // l < r if negative, l = r if 0, l > r if positive
}

// Joins two treeNodes
treeNode *join(treeNode *l, treeNode *r);

# endif
