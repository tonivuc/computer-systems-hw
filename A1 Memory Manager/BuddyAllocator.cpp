/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    /* This initializes the memory allocator and makes a portion of
    ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as
    its minimal unit of allocation. The function returns the amount of
    memory made available to the allocator. If an error occurred,
     it returns 0.
    */


	//Okay so I will have a LinkedList, that when traversed gives all the block sizes, and points to the header of the first
	//free block in a list of blocks



	/* Just some code I might use later
	 * char* h = (addr - sizeOfHeader);
	 * BlockHeader* header = (BlockHeader*)(h);
	 * cout << header->size << endl; //this would print out the size of the block that h points to
	 */

	//First use C++ function to allocate a part of the memory (total memory size to be used by allocator)
	char *memoryStart = new char [_total_memory_length]; //Remember to free this
	//Make a LinkedList outside of our memory, and populate it with the pointer to the first big BlockHeader in our memory
	LinkedList freeList;
	BlockHeader initialBlock(true,_basic_block_size);
	freeList.insert(&initialBlock); //is this correct?

	//For free blocks tracking, there will be a collection of linked lists called FreeLists, each list containing all free blocks of the same size

	//So, I guess I could have a LinkedList called freeList
	//Then this consists of other LinkedLists called ListStarters, which have the insert and remove functions
	//oooor I could have BlockHeaders for the 2nd tier
	//Why am I not using my own LinkedList to store all the FreeLists? Because the Header is different
	//Use C++ vector in stead

}

BuddyAllocator::~BuddyAllocator (){
	
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

