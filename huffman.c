# include "huffman.h"
# include "stack.h"
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

// New node, with symbols, leaf or not, a count associated with it
treeNode *newNode(uint8_t s, bool l, uint64_t c)
{
	treeNode *n = (treeNode *) malloc(sizeof(treeNode));
	n->symbol = s;
	n->count = c;
	n->leaf = l;
	n->left = NIL;
	n->right = NIL;
	return n;
}

// Dump a Huffman tree onto a file
void dumpTree(treeNode *t, int32_t file)
{
	if (t != NIL)
	{
		dumpTree(t->left, file); // Dump the left side of the tree
		dumpTree(t->right, file); // Dump the right side of the tree

		// If the node is a leaf, write a L followed by the symbol of the
		// node to the output file.
		if (t->leaf)
		{
			uint8_t symbol[2];
			symbol[0] = 'L';
			symbol[1] = t->symbol;
			write(file, symbol, 2);
		}
		// Otherwise, just write an I
		else
		{
			uint8_t symbol = 'I';
			write(file, &symbol, 1);
		}
	}
	return;
}

// Build a tree from the saved tree
treeNode *loadTree(uint8_t savedTree[], uint16_t treeBytes)
{
	stack *nodes = newStack();
	// Go through each element of the saved tree
	for (uint32_t i = 0; i < treeBytes; i += 1)
	{
		// If a L is read, the next element is a symbol for a leaf
		if (savedTree[i] == 'L')
		{
			i += 1; // Consider the next element as a symbol
			treeNode *entry = newNode(savedTree[i], true, 0);
			push(nodes, entry);
		}
		// If an I is read, the last two elements are the 
		// children of an interior node
		else if (savedTree[i] == 'I')
		{ 
			treeNode *childR = pop(nodes);
			treeNode *childL = pop(nodes);
			treeNode *entry = join(childL, childR);
			push(nodes, entry);
		}
	}
	treeNode *tree = pop(nodes); // Take the remaining node, the tree
	delStack(nodes);
	return tree;
}

// Step through a tree following the code
int32_t stepTree(treeNode *root, treeNode **t, uint32_t code)
{
	// Move left if the code is 0
	if (code == 0)
	{
		*t = root->left;
	}
	// Otherwise, move right
	else
	{
		*t = root->right;
	}

	// Return the symbol if a leaf has been reached
	if ((*t)->leaf)
	{
		return (*t)->symbol;
	}
	// Otherwise, inform that a leaf has not been detected
	else
	{
		return -1;
	}
}

// Parse a Huffman tree to build codes
void buildCode(treeNode *t, code s, code table[256])
{
	// If the current node is a leaf, then the
	// current code is athe code for the leaf
	if (t->leaf == true)
	{
		table[t->symbol] = s;
	}
	// Otherwise, continue to go through each node in the tree
	// and build the code along the way.
	else
	{
		pushCode(&s, 0);
		buildCode(t->left, s, table);
		uint32_t temp = 0;
		popCode(&s, &temp);
		pushCode(&s, 1);
		buildCode(t->right, s, table);
	}
	return;
}

// Delete a tree
void *delTree(treeNode *h)
{
	if (h != NIL)
	{
		delTree(h->left);
		delTree(h->right);
		delNode(h);
	}
	return h;
}

// Join two treeNodes
treeNode *join(treeNode *l, treeNode *r)
{
	treeNode *root = newNode('$', false, l->count + r->count);
	root->left = l;
	root->right = r;
	return root;
}
