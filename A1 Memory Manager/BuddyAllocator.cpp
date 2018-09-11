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
	_basic_block_size = returnClosestPowerOf2(_basic_block_size);
	vector<LinkedList> allFreeLists = initializeFreeLists(_basic_block_size, _total_memory_length);

	cout << "allFreeLists.size()  nr. 1 " << allFreeLists.size() << "\n";

	char *memoryStart = new char [_total_memory_length]; //Remember to free this


	BlockHeader* initialBlock = (BlockHeader*) memoryStart; //No point calling constructor before we have the right memory location
	allFreeLists[0].insert(initialBlock);
	cout << allFreeLists.size() << "\n";
	cout << " blocksiz " <<allFreeLists[0].getFirstHeader()->getBlocksize() << "\n";
	cout << " avSiz " <<allFreeLists[0].getFirstHeader()->getAvailableSize() << "\n";

	alloc(128);
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

char* BuddyAllocator::alloc(uint _length) {

	cout << "allFreeLists.size()" << getAllFreeLists().size();
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
	//Start at bottom of FreeList and see if there is a block that can fit the data?

	for (int i = allFreeLists.size(); i > 0; i--) {
		if (allFreeLists[i].getBlockSize() >= _length) {
			int timesToSplit = findNumSplits(allFreeLists[i].getBlockSize(), _length, 0);
			cout << "times to Split: " << timesToSplit;
		}
	}

	//Check if it can fit the data if split in two?
	//Keep checking and splitting until it can't get smaller
	//Allocate

	/*
	for (int i = allFreeLists.size(); i > 0; i--) {
		if ((allFreeLists[i].getFirstHeader() != NULL) && allFreeLists[i].getBlockSize() >= ) {

		}
	}
	 */
    return new char [_length];
}

/*
 * Recurring function to find how many times to split the memory
 */
int BuddyAllocator::findNumSplits(uint currBlockSize, uint dataLength, int splitsSoFar) {
	if (currBlockSize/2 >= dataLength /*minus header size*/) {
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

char *BuddyAllocator::split(char *block) {
	return nullptr;
}

const vector<LinkedList> &BuddyAllocator::getAllFreeLists() const {
	return allFreeLists;
}

void BuddyAllocator::setAllFreeLists(const vector<LinkedList> &allFreeLists) {
	BuddyAllocator::allFreeLists = allFreeLists;
}
