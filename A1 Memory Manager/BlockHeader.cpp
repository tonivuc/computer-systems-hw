//
// Created by Toni on 06.09.2018.
//

#include <iostream>
#include "BlockHeader.h"

BlockHeader::BlockHeader(bool free, int blocksize) {
    this->free = free;
    this->blocksize = blocksize;
	//I've calculated previously that the total size of the BlockHeader object is 12 bytes
}

BlockHeader::BlockHeader() {
	this->free = true;
	//Maybe also have the blocksize be set?
} //Default constructor

bool BlockHeader::isFree() const {
    return free;
}

void BlockHeader::setFree(bool free) {
    BlockHeader::free = free;
}

unsigned int BlockHeader::getBlocksize() {
    return blocksize;
}

void BlockHeader::setBlocksize(unsigned int blocksize) {
	BlockHeader::blocksize = blocksize;
}

BlockHeader *BlockHeader::getNextBlock() const {
	return nextBlock;
}

void BlockHeader::setNextBlock(BlockHeader *nextBlock) {
	BlockHeader::nextBlock = nextBlock;
}

int BlockHeader::getAvailableSize() {
	std::cout << "Size of BlockHeader: " << sizeof(this);
	return blocksize - sizeof(this);
}