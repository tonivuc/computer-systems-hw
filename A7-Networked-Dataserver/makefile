# makefile

all: dataserver client

requestchannel.o: requestchannel.h requestchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c requestchannel.cpp

network_req_channel.o: network_req_channel.h network_req_channel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c network_req_channel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp

dataserver: dataserver.cpp requestchannel.o network_req_channel.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp requestchannel.o network_req_channel.o -lpthread -lrt

client: client.cpp requestchannel.o BoundedBuffer.o Histogram.o network_req_channel.o
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp requestchannel.o BoundedBuffer.o Histogram.o network_req_channel.o -lpthread -lrt

clean:
	rm -rf *.o dataserver client
