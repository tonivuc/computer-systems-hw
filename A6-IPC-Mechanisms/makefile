# makefile

all: dataserver client

requestchannel.o: requestchannel.h requestchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c requestchannel.cpp

fifo_req_channel.o: fifo_req_channel.h fifo_req_channel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c fifo_req_channel.cpp

mq_req_channel.o: mq_req_channel.h mq_req_channel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c mq_req_channel.cpp

kernel_semaphore.o: kernel_semaphore.h kernel_semaphore.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c kernel_semaphore.cpp

shm_bounded_buffer.o: shm_bounded_buffer.h shm_bounded_buffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c shm_bounded_buffer.cpp

shm_req_channel.o: shm_req_channel.h shm_req_channel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c shm_req_channel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp

dataserver: dataserver.cpp requestchannel.o fifo_req_channel.o mq_req_channel.o kernel_semaphore.o shm_bounded_buffer.o shm_req_channel.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp requestchannel.o fifo_req_channel.o mq_req_channel.o kernel_semaphore.o shm_bounded_buffer.o shm_req_channel.o -lpthread -lrt

client: client.cpp requestchannel.o BoundedBuffer.o Histogram.o kernel_semaphore.o shm_bounded_buffer.o shm_req_channel.o
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp requestchannel.o BoundedBuffer.o Histogram.o fifo_req_channel.o mq_req_channel.o kernel_semaphore.o shm_bounded_buffer.o shm_req_channel.o -lpthread -lrt

clean:
	rm -rf *.o dataserver client
