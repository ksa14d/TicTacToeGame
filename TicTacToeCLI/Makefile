CC = g++
CFLAGS= -O3 -Wall -std=c++11 -pedantic 
LIBS = -lpthread\

all: server client

clean:
	rm -f *.o
	rm -f server 
	rm -f *~
	rm -f core

server.o: server.cpp
	$(CC) $(CFLAGS) -g -c server.cpp ${LIBS}

server:  server.o 
	$(CC) $(CFLAGS) -g  -o server server.o ${LIBS}
  
client.o: client.cpp
	$(CC) $(CFLAGS) -g -c client.cpp ${LIBS}

client:  client.o 
	$(CC) $(CFLAGS) -g  -o client client.o ${LIBS}
demo:	server
	./server 10000
	
