# makefile

all: dataserver client

requestchannel.o: requestchannel.h requestchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c requestchannel.cpp

fifo_request_channel.o: fifo_request_channel.h fifo_request_channel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c fifo_request_channel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp

dataserver: dataserver.cpp requestchannel.o fifo_request_channel.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp requestchannel.o fifo_request_channel.o -lpthread -lrt

client: client.cpp requestchannel.o BoundedBuffer.o Histogram.o
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp requestchannel.o BoundedBuffer.o Histogram.o fifo_request_channel.h -lpthread -lrt

clean:
	rm -rf *.o dataserver client
