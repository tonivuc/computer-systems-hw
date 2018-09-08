//
// Created by Toni on 06.09.2018.
//

#include "BlockHeader.h"

BlockHeader::BlockHeader(bool free, int blocksize) :  {
    this->free = free;
    this->blocksize = blocksize;
}

BlockHeader::BlockHeader() {} //Default constructor

bool BlockHeader::isFree() const {
    return free;
}

void BlockHeader::setFree(bool free) {
    BlockHeader::free = free;
}

void BlockHeader::setBlocksize(int blocksize) {
    BlockHeader::blocksize = blocksize;
}

int BlockHeader::getBlocksize() {
    return blocksize;
}
