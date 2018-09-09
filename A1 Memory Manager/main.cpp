/*#include "test/Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 512 * 1024;

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

    // test memory manager
    Ackerman* am = new Ackerman ();
    am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;
}
 */

#include "BlockHeader.h"
#include <new>
#include <iostream>
using namespace std;

int main() {

	//BlockHeader header; //If I remove this line, the Block size output is wrong. Even though I never reference this. What??

	int _total_memory_length = 10000; //byte

	char *memoryStart = new char [_total_memory_length];


	//RANDOM CODE TO BE REMOVED STARTS HERE
	//printf("Memory starts at: %0X \n",memoryStart);

	cout << "Memory starts at: " << &memoryStart << "\n";

	//ENDS HERE

	BlockHeader* b = (BlockHeader*) memoryStart; //I guess if I called the constructor it would
	b->setBlocksize(24);
	cout << "blocksize " << b->getBlocksize();

	/*
	cout << "Memory starts at: " << static_cast<void*>(memoryStart) << "\n";

	BlockHeader* b = (BlockHeader*) memoryStart;
	b->setBlocksize(200), b->setFree(free);
	//If I just use memoryStart here (no &), BlockHeader location: prints another address, why? I thought a plain pointer already gives me the memory address?
	//BlockHeader* headerLocation = new(&memoryStart) BlockHeader();

	cout << "BlockHeader location: " << b;
	cout << "\nBlock size: " << b->getBlocksize(); //Checking if the object is actually there

	delete b;
	*/
	return 0;
}