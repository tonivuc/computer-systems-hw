

int main() {
	
	long number = 1337;
	int _total_memory_length = 10000; //byte
	
	char *memoryStart = new char [_total_memory_length];
	
	char memory[sizeof(number)];     // Line #1
	void* place = memoryStart;          // Line #2
	long* numberLocation = new(place) long;  
	return 0;
	
}