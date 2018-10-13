# include "bv.h"
# include "huffman.h"
# include "stack.h"

# include <stdlib.h>
# include <stdio.h>
# include <errno.h>
# include <getopt.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

int main(int argc, char **argv)
{
    int32_t fileIn = -1; // Holds input file
    int32_t fileOut = STDOUT_FILENO; // Holds output file
    bool verbose = false; // Option to show statistics

    // Take in all of the command line arguments and act accordingly
    int32_t option;
    while ((option = getopt(argc, argv, "i:o:v")) != -1)
    {
        switch (option)
        {
            case 'i':
            {
                // Open an input file and exit if it failed
                fileIn = open(optarg, O_RDONLY);
                if (fileIn == -1)
                {
                    fprintf(stderr, "decode: ");
                    perror(optarg);
                    exit(errno);
                }
                break;
            }
            case 'o':
            {
                // Exit if the output file already exists
                fileOut = open(optarg, O_WRONLY);
                if (fileOut != -1)
                {
                    fprintf(stderr, "decode: ");
                    perror(optarg);
                    exit(errno);
                }
                // If it doesn't, then create it
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

    // Check to make sure the file has been encoded
    uint32_t magicNum = 0;
    if(read(fileIn, &magicNum, 4) == -1)
    {
        perror("decode");
        exit(errno);
    }
    if (magicNum != 0xdeadd00d)
    {
        fprintf(stderr, "Read of magic number failed.\n");
        exit(0);
    }

    // Read the size of the original file
    uint64_t fileSize = 0;
    if (read(fileIn, &fileSize, 8) == -1)
    {
        perror("decode");
        exit(errno);
    }

    // Read the size of the Huffman tree
    uint16_t treeSize = 0;
    if (read(fileIn, &treeSize, 2) == -1)
    {
        perror("decode");
        exit(errno);
    }

    // Read and store the saved Huffman tree
    uint8_t savedTree[treeSize];
    if (read(fileIn, savedTree, treeSize) == -1)
    {
        perror("decode");
        exit(errno);
    }

    // Reconstruct Huffman tree
    treeNode *tree = loadTree(savedTree, treeSize);

    uint64_t curSym = 0; // Count the amount of characters detected
    treeNode *curHead = tree; // Used to keep a relative position on the tree
    treeNode **curPos = &curHead; // Used to move around the tree
    // Go through each byte of the file
    uint8_t buf = 0;
    while (read(fileIn, &buf, 1) > 0 && curSym < fileSize)
    {
        // Go through each bit of the byte
        for (int32_t i = 0; i < 8; i += 1)
        {
            uint32_t val = (buf & 0x1 << i%8) >> i%8;

            curHead = *curPos;
            int32_t leaf;
            // Step left if 0 and right if 1
            if (val)
            {
                leaf = stepTree(curHead, curPos, 1);
            }
            else
            {
                leaf = stepTree(curHead, curPos, 0);
            }

            // If a leaf has been detected
            if (leaf != -1)
            {
                write(fileOut, &leaf, 1); // Write out the character
                curSym += 1; // One character read
                // Stop if you've read the original amount of characters
                if (curSym >= fileSize)
                {
                    break;
                }
                curHead = tree; // Start back at the top of the tree
            }
        }
    }

    // Huffman tree no longer needed
    delTree(tree);

    // Display statistics of decode if desired
    if (verbose)
    {
        printf("%lu bits: ", fileSize * 8);
        printf("tree (%u)\n", treeSize);
    }

    // Close the files that were used
    close(fileIn);
    if (fileOut != STDOUT_FILENO)
    {
        close(fileOut);
    }
    return 0;
}
