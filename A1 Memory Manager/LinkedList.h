//
// Created by Toni on 05.09.2018.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "BlockHeader.h"

class LinkedList{
    // this is a special linked list that is made out of BlockHeader s.
private:
    BlockHeader* firstHeader;		// you need a head of the list
	unsigned int blockSize = 128; //default 128 bytes. Might want to change this to char array later.

public:
	LinkedList(unsigned int blockSize);
	LinkedList();

	void insert (BlockHeader* b);// adds a block to the list

    void remove (BlockHeader* b); // removes a block from the list

	BlockHeader *getHead() const;

	void setHead(BlockHeader *head);

	BlockHeader *getFirstHeader() const;

	void setFirstHeader(BlockHeader *firstHeader);

	unsigned int getBlockSize() const;

	void setBlockSize(unsigned int blockSize);
};
#endif //A1_MEMORY_MANAGER_LINKEDLIST_H