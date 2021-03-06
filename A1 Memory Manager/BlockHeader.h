//
// Created by Toni on 05.09.2018.
//

/**
 * This header marks the start of a new memory block.
 */

#ifndef BLOCKHEADER_H
#define BLOCKHEADER_H

#include <stdio.h>

/*
An easy solution is to use the free memory blocks
themselves to store the free-list data. For example, the first bytes of each free memory block
would contain the pointer to the previous and to the next free memory block of the same
size. -Assignment paper
 */

class BlockHeader{
    // decide what goes here
    // hint: obviously block size will go here

private:
    bool free = true;
    unsigned int blocksize = 128; //byte
	BlockHeader* nextBlock = NULL;

public:
    BlockHeader(bool free, int blocksize);
    BlockHeader();

    bool isFree() const;
    void setFree(bool free);
    unsigned int getBlocksize();
	void setBlocksize(unsigned int blocksize);

	BlockHeader *getNextBlock() const;

	void setNextBlock(BlockHeader *nextBlock);

	int getAvailableSize();
};

#endif