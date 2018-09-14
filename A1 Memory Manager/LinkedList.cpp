//
// Created by Toni on 08.09.2018.
//

#include <iostream>
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

	std::cout << "using insert function in LinekedList\n";

	b->setNextBlock(firstHeader); //Set this block to point to the previous first block in the list
	//The order in the list isn't the order in the physical memory.
	b->setBlocksize(blockSize), b->setFree(true);
	firstHeader = b; //finally set firstHeader to point to the new header block
}

void LinkedList::remove(BlockHeader *b) {
	std::cout << "before calling getNextBlock()\n";
	//std::cout << "for b: " << b << " we have that b->getNextBlock() " << b->getNextBlock() <<"\n";
	//BlockHeader* header = firstHeader->getNextBlock();

	//node = b
	//temp = ?
	//Previous BlockHeader points to this address.
	//Move content from the next block into this block.
	//Overwrites this block, and nobody points to the next block any more.
	//In stead this block now points to the old next block's next block and the list is intact.
	//Moving the data from the next node into this node
	std::cout << "Hva er b->getNextBlock()?  det er: " << b->getNextBlock() << "\n";
	if (b->getNextBlock() != NULL) { //If it's not the last block in the LinkedList
		std::cout << "address: " << b << " has next block " << b->getNextBlock() << "\n";
		BlockHeader * temp = b->getNextBlock();
		b->setBlocksize(b->getNextBlock()->getBlocksize());
		b->setNextBlock(temp->getNextBlock());
	}
		//Next block is NULL.
	else {
		//Hva skal skje hvis det er siste blokk i listen?
		//Current block must be removed, by finding the element prior in the LinkedList and removing it

		BlockHeader* header = firstHeader;
		if (header == b) {
			std::cout << "We have reached the only block in the list. Remove it!\n";
			firstHeader = NULL;
		}
		else {
			int k = 0;
			while(header->getNextBlock() != b) {
				std::cout << "going through LinkedList, at index " << k << "\n";
				header = header->getNextBlock();
				k++;
				if (k == 10000) {
					std::cout << "We might have entered an infinite loop in the removal function folks \n";
				}
			}
			//Turns out the next block was the block we wanted to remove.
			header->setNextBlock(NULL);
			std::cout << "Removed block";
		}
	}
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


