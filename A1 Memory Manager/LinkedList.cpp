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
	BlockHeader* b = (BlockHeader*) memoryStart;
	b->size = x, b->free= true, b->next = NULL;
}

void LinkedList::remove(BlockHeader *b) {

}

BlockHeader *LinkedList::getFirstHeader() const {
	return firstHeader;
}

void LinkedList::setFirstHeader(BlockHeader *firstHeader) {
	LinkedList::firstHeader = firstHeader;
}

/*
LinkedList &LinkedList::getNextHead() {
	return nextListHead;
}
 */

/*
void LinkedList::setNextHead(LinkedList &nextHead) {
	LinkedList::nextListHead = nextHead;
}
 */

LinkedList *LinkedList::getNextListHead() const {
	return nextListHead;
}

void LinkedList::setNextListHead(LinkedList *nextListHead) {
	LinkedList::nextListHead = nextListHead;
}

unsigned int LinkedList::getBlockSize() const {
	return blockSize;
}

void LinkedList::setBlockSize(unsigned int blockSize) {
	LinkedList::blockSize = blockSize;
}


