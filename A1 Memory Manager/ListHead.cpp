//
// Created by Toni on 08.09.2018.
//

#include "ListHead.h"

ListHead::ListHead(const ListHead &nextHead, const BlockHeader &firstHeader, unsigned int blockSize) : nextHead(
		nextHead), firstHeader(firstHeader), blockSize(blockSize) {}

const ListHead &ListHead::getNextHead() const {
	return nextHead;
}

void ListHead::setNextHead(const ListHead &nextHead) {
	ListHead::nextHead = nextHead;
}

const BlockHeader &ListHead::getFirstHeader() const {
	return firstHeader;
}

void ListHead::setFirstHeader(const BlockHeader &firstHeader) {
	ListHead::firstHeader = firstHeader;
}

unsigned int ListHead::getBlockSize() const {
	return blockSize;
}

void ListHead::setBlockSize(unsigned int blockSize) {
	ListHead::blockSize = blockSize;
}
