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


	/* Just some code I might use later
	 * char* h = (addr - sizeOfHeader);
	 * BlockHeader* header = (BlockHeader*)(h);
	 * cout << header->size << endl; //this would print out the size of the block that h points to
	 */
	_basic_block_size = returnClosestPowerOf2(_basic_block_size+sizeof(BlockHeader));
	allFreeLists = initializeFreeLists(_basic_block_size, _total_memory_length);

	cout << "Size to be allocated: " << _basic_block_size << "\n";

	cout << "allFreeLists.size()  nr. 1 " << allFreeLists.size() << "\n";

	memoryStart = new char [_total_memory_length]; //Remember to free this


	BlockHeader* initialBlock = (BlockHeader*) memoryStart; //No point calling constructor before we have the right memory location
	allFreeLists[0].insert(initialBlock);
	cout << allFreeLists.size() << "\n";
	cout << " blocksiz " <<allFreeLists[0].getFirstHeader()->getBlocksize() << "\n";
	//cout << " avSiz " <<allFreeLists[0].getFirstHeader()->getAvailableSize() << "\n";

	alloc(_basic_block_size, allFreeLists);
}

BuddyAllocator::~BuddyAllocator (){
	//Destructor
}

vector<LinkedList> BuddyAllocator::initializeFreeLists(unsigned int _basic_block_size, unsigned int _total_memory_length) {
	vector<LinkedList> allFreeLists;
	int blockLength = _total_memory_length;

	for (int i = 1; blockLength > _basic_block_size; i = i*2) {
		//cout << "EEEy lmao in the function w. i= " << i << "\n";
		blockLength = _total_memory_length/i;
		//cout << "Blocklength= " << blockLength << "\n";
		allFreeLists.push_back(LinkedList(blockLength));
	}
	return allFreeLists;
}

char* BuddyAllocator::alloc(uint _length, vector<LinkedList> allFreeLists) {
	cout << "Alloc running!";
	int timesToSplit = 0;
	cout << "allFreeLists.size()" << allFreeLists.size() << "\n";
	BlockHeader* blockToSplit;
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */

	//Start at bottom of FreeList and see if there is a block that can fit the data
	for (int i = allFreeLists.size()-1; i > -1; i--) {
		//cout << "allFreeLists[i].getFirstHeader() " << allFreeLists[i].getFirstHeader() << "\n";
		if ((allFreeLists[i].getBlockSize() >= _length) && (allFreeLists[i].getFirstHeader() != NULL)) {
			cout << "\n allFreeLists[i].getBlockSize(): at i =  "<< i<< " s " << allFreeLists[i].getBlockSize() <<"\n";
			cout << "We found a block that's big enough for 128!";
			timesToSplit = findNumSplits(allFreeLists[i].getBlockSize(), _length, 0);
			//So if we don't have to split, we just return the memory address (as a char pointer)

			blockToSplit = allFreeLists[i].getHead();
			cout << " times to Split: " << timesToSplit;

			char* retrnAddr;
			for(int j = 0; j < timesToSplit; j++) {
				retrnAddr = split((char*)(blockToSplit));
			}
			BlockHeader* bluck = (BlockHeader*)retrnAddr;
			cout << "Size of last block: " << bluck->getBlocksize()<<"\n";

			return (char*)bluck; //REMEMBER TO OFFSET

		}
	}

	//Remember! Offset return address by header size
    return new char [_length];
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
  /* Same here! */
  delete _a;
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
	char * buddyAddress = (char*)((addr-memoryStart)^(blockheader->getBlocksize()) + (uint)memoryStart); //This is correct, just gotta find startAddress.

	return nullptr;
}

bool BuddyAllocator::isvalid(char *addr) {
	return false;
}

bool BuddyAllocator::arebuddies(char *block1, char *block2) {
	return false;
}

char *BuddyAllocator::merge(char *block1, char *block2) {
	return nullptr;
}

char *BuddyAllocator::split(char *blockAddress) {
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected

	//we get a char pointer to the big block
	//In allFreeList I should only insert the blocks to the right, never the most leftmost block in the branch
	//How to find the memory location of the half-block?
	BlockHeader* bigBlock = (BlockHeader*)blockAddress;
	int bigBlockSize = bigBlock->getBlocksize();
	int halfSize = bigBlockSize/2;
	char * rightBlockAddr = blockAddress + halfSize;

	cout << "Still no getNextBlock()?\n";

	//Can I look up the right FreeList in constant time?
	//I have halfSize, which is the size of the FreeList we will insert into
	//Look into that after I make this work...

	for (int i = 0; i < allFreeLists.size(); i++) {
		//Delete big block
		if (allFreeLists[i].getBlockSize() == bigBlockSize) {
			cout << "\nCalling remove of block " << allFreeLists[i].getHead() << "\n";
			allFreeLists[i].remove((BlockHeader*)blockAddress);
			cout << "After first remove!\n";
			//Insert new blocks
			allFreeLists[i+1].insert(bigBlock); //Insert the left block (It's no longer acutally big, size is set in the insert function)
			allFreeLists[i+1].insert((BlockHeader*)rightBlockAddr); //Insert the right block
			return rightBlockAddr; //Return pointer to new header (the block to the right)
		}
	}
	//If error
	return nullptr;
}

const vector<LinkedList> &BuddyAllocator::getAllFreeLists() const {
	return allFreeLists;
}

void BuddyAllocator::setAllFreeLists(const vector<LinkedList> &allFreeLists) {
	BuddyAllocator::allFreeLists = allFreeLists;
}
