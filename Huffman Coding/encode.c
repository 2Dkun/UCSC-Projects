# include "code.h"
# include "bv.h"
# include "huffman.h"
# include "queue.h"

# include <stdlib.h>
# include <errno.h>
# include <stdio.h>
# include <getopt.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

int main(int argc, char **argv)
{
	uint32_t histogram[256]; // Used to count the amount of each character
	int32_t fileIn = -1; // Holds the input file
	int32_t fileOut = STDOUT_FILENO; // Holds the outputfile
	char *fileName; // Holds the name of the input file
	bool verbose = false; // Option to show statistics

	// Go through all of the command line arguments and act accordingly
	int32_t option;
	while ((option = getopt(argc, argv, "i:o:v")) != -1)
	{
		switch (option)
		{
			case 'i':
			{
				// Open the input file and exit if it fails
				fileName = strdup(optarg); // Store the name of the input file
				fileIn = open(fileName, O_RDONLY);
				if (fileIn == -1)
				{
					fprintf(stderr, "encode: ");
					perror(optarg);
					exit(errno);
				}
				break;
			}
			case 'o':
			{
				// Exit if the output file exists
				fileOut = open(optarg, O_WRONLY);
				if (fileOut != -1)
				{
					fprintf(stderr, "encode: ");
					perror(optarg);
					exit(0);
				}
				// Else create the output file
				fileOut = open(optarg, O_WRONLY | O_CREAT, 0666);
				break;
			}
			case 'v':
			{
				// Show statistics
				verbose = true;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	// Stop the program if there was no input file to encode
	if (fileIn == -1)
	{
		fprintf(stderr, "There was no input file to encode.\n");
		exit(0);
	}

	// Initialize the histogram
	histogram[0] = 1; // Set count as 1 for at least two numbers
	histogram[255] = 1;
	for (uint32_t i = 1; i < 255; i += 1)
	{
		// Set the rest as 0
		histogram[i] = 0;
	}

	// Read the input file and keep a count of each character
	uint64_t totalBytes = 0; // Keep track of the total bytes read
	// Read 1024 bytes at a time and stop if less bytes were read
	ssize_t bytesRead = 1024;
	while (bytesRead >= 1024)
	{
		uint8_t buf[1024];
		bytesRead = read(fileIn, buf, 1024);
		if (bytesRead == -1) // Exit if read fails
		{
			perror("encode");
			exit(errno);
		}
		totalBytes += bytesRead; // Update the total bytes read

		// Increase the corresponding histogram element by one for each byte read
		for (uint32_t i = 0; i < bytesRead; i += 1)
		{
			histogram[buf[i]] += 1;
		}
	}

	// Place all of the characters that were in the file into a priority queue
	uint16_t leaves = 0; // Count the amount of unique symbols
	queue *priorities = newQueue(257); // Used to store the nodes of a Huffman tree
	for (uint16_t i = 0; i < 256; i += 1)
	{
		// If there's at least one of the symbol in the histogram,
		// make a leaf out of it and put it into a priority queue
		if (histogram[i] > 0)
		{
			leaves += 1;
			treeNode *entry = newNode(i, true, histogram[i]);
			enqueue(priorities, entry);
		}
	}

	// Create a David Huffman tree using the priority queue
	treeNode *tree; // Holds the Huffman tree
	// Keep joining the two smallest nodes until there are none left
	while (true)
	{
		treeNode *left = NULL, *right = NULL;
		// If there was nothing left in the queue, then 
		// the last joined node is the tree.
		if (!dequeue(priorities, &left))
		{
			break;
		}
		// If there wasn't a second node left, the previous
		// node is the tree.
		if (!dequeue(priorities, &right))
		{
			tree = left;
			break;
		}
		tree = join(left, right); // Join the two nodes
		enqueue(priorities, tree); // Put the joined node back in the queue
	}

	// Create the code for each symbol in the Huffman tree
	code chart[256]; // Holds the code for each symbol
	code s = newCode(); // Stores a single code
	buildCode(tree, s, chart); // Create a code for each symbol

	// Write a number into the output so you can tell it's already encoded
	uint32_t magicNum = 0xdeadd00d;
	write(fileOut, &magicNum, 4);

	// Write the length of the original file
	write(fileOut, &totalBytes, 8);

	// Write the size of the Huffman tree
	uint32_t huffSize = (3 * leaves) - 1;
	write(fileOut, &huffSize, 2);

	// Write the Huffman tree to the file using post-order traversal
	dumpTree(tree, fileOut);

	// Write the encoding of the original file onto the new one
	bitV *bits = newVec(8); // Stores all of the bits of each code
	close(fileIn); // Close the input file and open it again to read it again
	fileIn = open(fileName, O_RDONLY);
	uint8_t buf = 0; // Used to take in one byte of input
	uint32_t encodedBits = 0; // Count the amount of bits encoded
	// Read each byte of the input file
	while (read(fileIn, &buf, 1) > 0)
	{
		code curSym = chart[buf]; // Obtain the code for the byte read
		uint32_t fixedCode[8]; // Used to keep the order of the code
		uint32_t codePos = 0; // Used to keep the position of the code
	
		// Find the value of each bit in the code and store it in order
		uint32_t value = 0;
		while (popCode(&curSym, &value))
		{
			fixedCode[codePos] = value; // Store the bit
			codePos += 1; // Increment the position by one
		}

		// Store the sorted bit into the bit vector
		for (int32_t i = codePos - 1; i >= 0; i -= 1)
		{
			encodedBits += 1; // One bit was encoded

			// Each time a byte has been filled out, write the
			// bit vector to the output and reset the bit vector
			if (setNextBit(bits, fixedCode[i]))
			{
				write(fileOut, bits->vector, 1);
				cleanVec(bits);
			}
		}
	}
	// Write out the remaining bits if there were any
	if (bits->position != 0)
	{
		write(fileOut, bits->vector, 1);
	}

	// Display the statistics of the encoding if desired
	if (verbose)
	{
		printf("Original %lu bits: ", totalBytes * 8);
		printf("leaves %hu (%u bytes)", leaves, huffSize);
		double ratio = ((double) encodedBits) / totalBytes;
		printf("encoding %u bits (%.4f%%).\n", encodedBits, ratio);
	}

	// Deallocate the dynamically allocated objects
	delTree(tree);
	delQueue(priorities);
	delVec(bits);
	free(fileName);

	// Close the files that were used
	close(fileIn);
	if (fileOut != STDOUT_FILENO)
	{
		close(fileOut);
	}
	return 0;
}
