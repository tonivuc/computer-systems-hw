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

	vector<LinkedList> allFreeLists = initializeFreeLists(_basic_block_size, _total_memory_length)


	_basic_block_size = returnClosestPowerOf2(_basic_block_size);

	//First use C++ function to allocate a part of the memory (total memory size to be used by allocator)
	//Reserve some space for the FreeLists



	char *memoryStart = new char [_total_memory_length]; //Remember to free this
	//Make a LinkedList outside of our memory, and populate it with the pointer to the first big BlockHeader in our memory



	BlockHeader* b = (BlockHeader*) memoryStart;
	b->size = x, b->free= true, b->next = NULL;

	BlockHeader initialBlock(true,_basic_block_size); //No point calling constructor before we have the right memory location
	allFreeLists[0].insert(&initialBlock); //is this correct?

	BlockHeader* header = new BlockHeader(true,_total_memory_length);

	//For free blocks tracking, there will be a collection of linked lists called FreeLists, each list containing all free blocks of the same size
	//Initially I'm just creating the first block.
	//allFreeLists vector size can be whatever

}

BuddyAllocator::~BuddyAllocator (){
	//Destructor
}

vector<LinkedList> BuddyAllocator::initializeFreeLists(unsigned int _basic_block_size, unsigned int _total_memory_length) {
	vector<LinkedList> allFreeLists;
	int blockLength = _total_memory_length;
	//allFreeLists.reserve(5);
	for (int i = 1; blockLength >= _basic_block_size; i*2) {
		blockLength = blockLength/i;
		allFreeLists.push_back(LinkedList(blockLength));
	}
	return allFreeLists;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  return new char [_length];
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

int BuddyAllocator::returnClosestPowerOf2(unsigned int x) {
	return (int)pow(2, ceil(log(x)/log(2))); //Source: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2 09.08.2018
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
