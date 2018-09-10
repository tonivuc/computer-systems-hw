//
// Created by Toni on 08.09.2018.
//

#include "LinkedList.h"

LinkedList::LinkedList() {}

LinkedList::LinkedList(unsigned int blockSize) {
	this->blockSize = blockSize;
}

BlockHeader *LinkedList::getHead() const {
	return firstHeader;
}

void LinkedList::setHead(BlockHeader *head) {
	LinkedList::firstHeader = head;
}

void LinkedList::insert(BlockHeader *b) {
	//Change the nextHeader variable
	//BlockHeader* b = (BlockHeader*) memoryStart;

	b->setNextBlock(firstHeader); //Set this block to point to the previous first block in the list
	//The order in the list isn't the order in the physical memory.
	b->setBlocksize(blockSize), b->setFree(true), firstHeader = b; //finally set firstHeader to point to the new header block
}

void LinkedList::remove(BlockHeader *b) {

}

BlockHeader *LinkedList::getFirstHeader() const {
	return firstHeader;
}

void LinkedList::setFirstHeader(BlockHeader *firstHeader) {
	LinkedList::firstHeader = firstHeader;
}

unsigned int LinkedList::getBlockSize() const {
	return blockSize;
}

void LinkedList::setBlockSize(unsigned int blockSize) {
	LinkedList::blockSize = blockSize;
}


