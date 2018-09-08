//
// Created by Toni on 05.09.2018.
//

/**
 * This header marks the start of a new memory block.
 */

#ifndef A1_MEMORY_MANAGER_BLOCKHEADER_H
#define A1_MEMORY_MANAGER_BLOCKHEADER_H

#endif //A1_MEMORY_MANAGER_BLOCKHEADER_H

class BlockHeader{
    // decide what goes here
    // hint: obviously block size will go here

private:
    bool free = true;
    int blocksize = 128; //byte

public:
    BlockHeader(bool free, int blocksize);
    BlockHeader();

    bool isFree() const;
    void setFree(bool free);
    void setBlocksize(int blocksize);
    int getBlocksize();
};

