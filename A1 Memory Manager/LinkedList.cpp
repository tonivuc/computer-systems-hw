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

	std::cout << "Inserting block with address "<<b<<"\n";

	b->setNextBlock(firstHeader); //Set this block to point to the previous first block in the list
	//The order in the list isn't the order in the physical memory.
	b->setBlocksize(blockSize), b->setFree(true);
	firstHeader = b; //finally set firstHeader to point to the new header block
}

void LinkedList::remove(BlockHeader *b) {
	std::cout << "Removing block"<< b<<"\n";
	//Previous BlockHeader points to this address.
	//Move content from the next block into this block.
	//Overwrites this block, and nobody points to the next block any more.
	//In stead this block now points to the old next block's next block and the list is intact.
	//Moving the data from the next node into this node
	if (b->getNextBlock() != NULL) { //If it's not the last block in the LinkedList
		std::cout << "address: " << b << " has next block " << b->getNextBlock() << "\n";
		BlockHeader * temp = b->getNextBlock();
		b->setBlocksize(b->getNextBlock()->getBlocksize());
		b->setNextBlock(temp->getNextBlock());
	}
		//Next block is NULL.
	else {
		//What if it is the last block in the LinkedList?
		//Current block must be removed, by finding the element prior in the LinkedList and removing it

		BlockHeader* firstRealBlock = firstHeader;
		if (firstRealBlock == b) {
			std::cout << "We have reached the only block in the list. That's " << firstRealBlock << "Remove it!\n";
			firstHeader = NULL; //The list no longer points to any blocks
		}
		else {
			bool stop_here = false;
			if (firstRealBlock == NULL) {
				std::cout << "The header of the LinkedList is pointing nowhere... This block is as good as removed already.";
				stop_here = true;
			}
			if (stop_here == false) {
				std::cout << "We have reached the last block in the LinkedList, but not the only one. The block is "<<firstRealBlock<<"\n";
				int k = 0;
				while((firstRealBlock->getNextBlock() != b)) {
					//std::cout << "going through LinkedList, at index " << k << "\n";
					firstRealBlock = firstRealBlock->getNextBlock();
					k++;
					if (k == 1000) {
						std::cout << "Block "<<b<<" was not found and could therefore not be removed."<<" \n";
						return;
					}
				}
				std::cout << "We found that that the first block in the LinkedList, " <<firstRealBlock << " points to the block we want to remove.\n";
				std::cout << "Removing "<<firstRealBlock->getNextBlock()<<"\n";
				//Turns out the next block was the block we wanted to remove.
				firstRealBlock->setNextBlock(NULL);
			}
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


