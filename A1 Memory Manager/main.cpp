#include "test/Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 512 * 1024;

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

	char* block1 = allocator->alloc(90);
	char* block2 = allocator->alloc(90);

	allocator->free(block1);


    // test memory manager
    //Ackerman* am = new Ackerman ();
    //am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;
}