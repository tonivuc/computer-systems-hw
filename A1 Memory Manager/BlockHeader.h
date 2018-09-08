//
// Created by Toni on 05.09.2018.
//

/**
 * This header marks the start of a new memory block.
 */

#ifndef A1_MEMORY_MANAGER_BLOCKHEADER_H
#define A1_MEMORY_MANAGER_BLOCKHEADER_H

#endif //A1_MEMORY_MANAGER_BLOCKHEADER_H

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
	//The point here is that the blocks should be next to each other in the memory (must be virtual memory, since real one is abstracted away)
	//So we need to explicitly set the address?
	//We have to find the starting address of our memory, and put the first block there
	BlockHeader *

public:
    BlockHeader(bool free, int blocksize);
    BlockHeader();

    bool isFree() const;
    void setFree(bool free);
    void setBlocksize(int blocksize);
    int getBlocksize();
};

