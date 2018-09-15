#include "test/Ackerman.h"

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 512 * 1024; //currently 128MB, before: 512 * 1024

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

    // test memory manager
    Ackerman* am = new Ackerman ();
    am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;


	/* Some testing code
	char* block1 = allocator->alloc(10);
	char* block2 = allocator->alloc(22609);
	cout << "block1 data is stored at "<<(void*)block1<<" and block2 data is stored at "<<(void*)block2<<"\n";
	//allocator->free(block1);
	cout << "Done with block1\n";
	allocator->free(block2);
	*/
}