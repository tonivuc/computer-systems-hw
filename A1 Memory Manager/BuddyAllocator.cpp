/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "LinkedList.h"
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    /* This initializes the memory allocator and makes a portion of
    ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as
    its minimal unit of allocation. The function returns the amount of
    memory made available to the allocator. If an error occurred,
     it returns 0.
    */
	if (_basic_block_size >= _total_memory_length) {
		cout << "The basic block size must be smaller than the total memory length!\n";
		return;
	}

	totalMemory = _total_memory_length;
	this->_basic_block_size = returnClosestPowerOf2(_basic_block_size);

	cout << "_basic_block_size inside the constructor "<<_basic_block_size<<"\n";
	allFreeLists = initializeFreeLists(_basic_block_size, _total_memory_length);

	cout << "Size to be allocated: " << _total_memory_length<< "\n";

	memoryStart = new char [_total_memory_length]; //Remember to free this


	BlockHeader* initialBlock = (BlockHeader*) memoryStart; //No point calling constructor before we have the right memory location
	allFreeLists[0].insert(initialBlock);
	cout << allFreeLists.size() << "\n";
	cout << " blocksize " <<allFreeLists[0].getFirstHeader()->getBlocksize() << "\n";
	//cout << " avSiz " <<allFreeLists[0].getFirstHeader()->getAvailableSize() << "\n";

}

BuddyAllocator::~BuddyAllocator (){
	//Destructor
	delete [] memoryStart;
}

vector<LinkedList> BuddyAllocator::initializeFreeLists(unsigned int _basic_block_size, unsigned int _total_memory_length) {
	vector<LinkedList> allFreeLists;
	unsigned int blockLength = _total_memory_length;

	for (unsigned int i = 1; blockLength > _basic_block_size; i = i*2) {
		//cout << "EEEy lmao in the function w. i= " << i << "\n";
		blockLength = _total_memory_length/i;
		//cout << "Blocklength= " << blockLength << "\n";
		allFreeLists.push_back(LinkedList(blockLength));
	}
	return allFreeLists;
}

char* BuddyAllocator::alloc(uint _length) {
	//cout << "_length is "<<_length<<" and _basic_block_sie is "<<_basic_block_size<<"\n";
	_length = returnClosestPowerOf2(_length+sizeof(BlockHeader));
	if (_length < _basic_block_size) {
		_length = _basic_block_size;
	}

	if (_length > totalMemory- sizeof(BlockHeader)) {
		cout << "Tried to allocate more memory than total available memory!";
		return nullptr;
	}

	int timesToSplit = 0;
	BlockHeader* blockToSplit;

	//Start at bottom of FreeList and see if there is a block that can fit the data
	//cout << "Trying to allocate "<<_length<<" bytes of memory.";
	for (int i = allFreeLists.size()-1; i > -1; i--) {
		cout << "available sizes are: "<<allFreeLists[i].getBlockSize()<< "\n";
		if ((allFreeLists[i].getBlockSize() >= _length) && (allFreeLists[i].getFirstHeader() != NULL)) {
			cout << "\n allFreeLists[i].getBlockSize(): at i =  "<< i<< " size " << allFreeLists[i].getBlockSize() <<"\n";
			cout << "We found a block that's big enough for " << _length << " !\n";
			timesToSplit = findNumSplits(allFreeLists[i].getBlockSize(), _length, 0);
			//timesToSplit = timesToSplit-1; //Might need to remove this
			//So if we don't have to split, we just return the memory address (as a char pointer)

			blockToSplit = allFreeLists[i].getHead();
			cout << " times to Split: " << timesToSplit<<"\n";

			BlockHeader* retrnAddr;
			if (timesToSplit > 0) {
				for(int j = 0; j < timesToSplit; j++) {
					retrnAddr = (BlockHeader*)split((char*)(blockToSplit));
				}
			}
			else {
				//In the FreeList, there should be a block of the right side, inserted by Split.
				cout << "allFreeLists[i].getHead() "<<allFreeLists[i].getHead()<<"\n";
				cout << "allFreeLists[i].getHead()->getNextBlock() "<<allFreeLists[i].getHead()->getNextBlock()<<"\n";
				cout << "allFreeLists[i].getHead() get size " << allFreeLists[i].getHead()->getBlocksize()<<"\n";
				retrnAddr = allFreeLists[i].getHead();
				allFreeLists[i].remove(retrnAddr);
				//The only reason this has a head, is because an element has been inserted, promting the list to have its head set to the address of
				//that element.
				//If we are inside this FreeList, it means the header was not equal to NULL.
				//I need the address where to put this next block.

				//retrnAddr = allFreeLists[i].getFirstHeader();
			}
			cout << "Size of block to be returned: " << retrnAddr->getBlocksize()<<"\n";
			return (char*)((int)retrnAddr+sizeof(BlockHeader)); //REMEMBER TO OFFSET
		}
	}
	//Didn't find a block
	cout << "No available blocks that are big enough for the requested memory chunk!\n";
	return nullptr;
}

/*
 * Recurring function to find how many times to split the memory
 */
int BuddyAllocator::findNumSplits(uint currBlockSize, uint dataLength, int splitsSoFar) {
	//cout << "currBlockSize/2 " << currBlockSize/2 << "dataLength" << dataLength << "\n";
	if (currBlockSize/2 >= dataLength /*minus header size*/) {
		//cout << "currBlockSize: " << currBlockSize << "\n";
		return findNumSplits(currBlockSize/2, dataLength, (splitsSoFar+1));
	}
	return splitsSoFar;
}

int BuddyAllocator::free(char* _a) { //free() function does not give you the size of the block

	BlockHeader* header = (BlockHeader*)((int)_a - sizeof(BlockHeader)); //The memory location the user got is not the header location. Find header.
	cout << "Freeing memory location "<<(void*)_a<<" which belongs to block "<<header<<"\n";
	int blockSize = header->getBlocksize();
	//User is done with a piece of memory and frees it
	//if its buddy is free, merge them
	//If buddy is not free, simply insert a block of the size into the corresponding FreeList

	char* buddyBlock = getbuddy((char*)header);
	BlockHeader* test = (BlockHeader*)buddyBlock;

	if(((BlockHeader*)buddyBlock)->isFree()) {
		merge(_a,buddyBlock);
	}
	else {
		//Buddy is not free
		//We will insert a BlockHeader of the same size as the freed memory
		for (int i = allFreeLists.size()-1; i > -1; i--) {
			//Find a FreeList with the correct size
			if (allFreeLists[i].getBlockSize() == blockSize) {
				allFreeLists[i].insert(header);
			}
		}
	}

	return 0;
}

char* BuddyAllocator::allocForAcker(uint _length) {
	/* This preliminary implementation simply hands the call over the
	   the C standard library!
	   Of course this needs to be replaced by your implementation.
	*/
	return new char [_length]; //pointer-variable = new data-type;
}

int BuddyAllocator::freeForAcker(char* _a) { //free() function does not give you the size of the block
	/* Same here! */
	delete _a;
	return 0;
}

void BuddyAllocator::debug (){
  
}

unsigned int BuddyAllocator::returnClosestPowerOf2(unsigned int x) {
	return (unsigned int)pow(2, ceil(log(x)/log(2))); //Source: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2 09.08.2018
}

//The private functions we are required to implement
char *BuddyAllocator::getbuddy(char *addr) {
	//Aoff = A – head_pointer, Aoff’ = Aoff XOR A->block_size
	//Buddy address = (block address – start) XOR (block size) + start
	//C++ operator XOR: ^

	BlockHeader* blockheader = (BlockHeader*) addr;

	int kek = (addr-memoryStart)^(blockheader->getBlocksize());
	char * buddyAddress = kek + memoryStart; //- returns int, adding a char pointer to an int returns a char pointer
	return buddyAddress;
}

bool BuddyAllocator::isvalid(char *addr) {
	return false;
}

bool BuddyAllocator::arebuddies(char *block1, char *block2) {
	if (getbuddy(block1) == block2) {
		return true;
	}
	return false;
}

char *BuddyAllocator::merge(char *block1, char *block2) {
	//Assume that it has already been checked if they are buddies, as the only function that calls merge is free()

	BlockHeader* block1H = (BlockHeader*)block1;
	BlockHeader* block2H = (BlockHeader*)block2;

	//Find the place where we insert the new block
	int newSize = block1H->getBlocksize()*2;

	for (int i = 0; i < allFreeLists.size(); i++) {
		if (allFreeLists[i].getBlockSize() == newSize) {
			if (block1H < block2H) {
				allFreeLists[i].insert(block1H);
			}
			else {
				allFreeLists[i].insert(block2H);
			}
		}
		break;
	}

	for (int i = 0; i < allFreeLists.size(); i++) {
		if (allFreeLists[i].getBlockSize() == block1H->getBlocksize()) {
			if (block1H < block2H) {
				allFreeLists[i].remove(block1H);
			}
			else {
				allFreeLists[i].remove(block2H);
			}

		}
		break;
	}

	if (block1 < block2) {
		return block1;
	}
	else {
		return block2;
	}
}

char *BuddyAllocator::split(char *blockAddress) {
	cout << "\nWe are splitting block " << (BlockHeader*)blockAddress << "which has the size "<<((BlockHeader*)blockAddress)->getBlocksize()<< "\n";
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected

	//we get a char pointer to the big block
	//In allFreeList I should only insert the blocks to the right, never the most leftmost block in the branch
	BlockHeader* bigBlock = (BlockHeader*)blockAddress;
	int bigBlockSize = bigBlock->getBlocksize();
	int halfSize = bigBlockSize/2;
	char * rightBlockAddr = blockAddress + halfSize;

	//Can I look up the right FreeList in constant time?
	//I have halfSize, which is the size of the FreeList we will insert into
	//Look into that after I make this work...

	for (int i = 0; i < allFreeLists.size(); i++) {
		cout << "allFreeLists["<<i<<"].getBlockSize()"<<allFreeLists[i].getBlockSize()<<"\n";
		//Delete big block
		if (allFreeLists[i].getBlockSize() == bigBlockSize) {
			//cout << "HalfSize nowdays is "<<halfSize<<"\n";
			//When we Split, we want to remove the block we are splitting, and replace it with two smaller blocks in its memory
			cout << "Calling remove of block " << (BlockHeader*)blockAddress << "\n";
			allFreeLists[i].remove((BlockHeader*)blockAddress);
			//Insert new blocks

			allFreeLists[i+1].insert(bigBlock); //Insert the left block (It's no longer acutally big, size is set in the insert function)
			allFreeLists[i+1].insert((BlockHeader*)rightBlockAddr); //Insert the right block
			cout << "The two new inserted blocks have the addresses "<< bigBlock << " and " << (BlockHeader*)rightBlockAddr << " with sizes " << bigBlock->getBlocksize() << " and "<<((BlockHeader*)rightBlockAddr)->getBlocksize() << " and "<< bigBlock->isFree()<< " and " << ((BlockHeader*)rightBlockAddr)->isFree()<< "\n";
			return rightBlockAddr; //Return pointer to new header (the block to the right)
		}
	}
	//If error
	cout << "ERROR IN SPLIT!";
	return nullptr;
}

const vector<LinkedList> &BuddyAllocator::getAllFreeLists() const {
	return allFreeLists;
}

void BuddyAllocator::setAllFreeLists(const vector<LinkedList> &allFreeLists) {
	BuddyAllocator::allFreeLists = allFreeLists;
}
