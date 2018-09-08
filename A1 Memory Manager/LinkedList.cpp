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
}

void LinkedList::remove(BlockHeader *b) {

}

BlockHeader *LinkedList::getFirstHeader() const {
	return firstHeader;
}

void LinkedList::setFirstHeader(BlockHeader *firstHeader) {
	LinkedList::firstHeader = firstHeader;
}

LinkedList &LinkedList::getNextHead() {
	return nextListHead;
}

void LinkedList::setNextHead(LinkedList &nextHead) {
	LinkedList::nextListHead = nextHead;
}

unsigned int LinkedList::getBlockSize() const {
	return blockSize;
}

void LinkedList::setBlockSize(unsigned int blockSize) {
	LinkedList::blockSize = blockSize;
}
