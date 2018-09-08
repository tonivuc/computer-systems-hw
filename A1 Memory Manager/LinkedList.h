//
// Created by Toni on 05.09.2018.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H



#include "BlockHeader.h"
/*
 * From Piazza:
 * In class and Lab, it was suggested to implement FreeList as an array of pointers where each cell point to a LinkedList
 * for that respective size of memory. Each cell of the FreeList array corresponds to a size of memory that could be allocated
 * and the LinkedList keeps track of how many of those blocks have been allocated.
 * ~ An instructor (Tanzir Ahmed) endorsed this answer  ~
 *
 * That means I will not use this class, but rather an array of pointers.
 */

class LinkedList{
    // this is a special linked list that is made out of BlockHeader s.
private:
    BlockHeader* firstHeader;		// you need a head of the list
	LinkedList* nextListHead;
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

	//LinkedList &getNextHead();

	//void setNextHead(LinkedList &nextHead);
	LinkedList *getNextListHead() const;

	void setNextListHead(LinkedList *nextListHead);

	unsigned int getBlockSize() const;

	void setBlockSize(unsigned int blockSize);
};
#endif //A1_MEMORY_MANAGER_LINKEDLIST_H