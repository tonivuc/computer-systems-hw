#include "test/Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 512 * 1024;

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);


	char* block1 = allocator->alloc(90);
	cout << "OKAY NEXT ALLOC COMING UP!!!!\n\n\n\n";
	char* block2 = allocator->alloc(90);

	cout << "block1 data is stored at "<<(void*)block1<<" and block2 data is stored at "<<(void*)block2<<"\n";
	allocator->free(block1);
	cout << "Done with block1\n";
	allocator->free(block2);


    // test memory manager
    //Ackerman* am = new Ackerman ();
    //am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;
}