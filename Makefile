CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpthread
TERM=x-terminal-emulator

PSHARED=./shared/
PSERVER=./msg_server/
PCLIENT=./msg_client/

all: shared.o $(PSERVER)inbox.o $(PCLIENT)client.o $(PSERVER)server.o 
	$(CC) $(PCLIENT)shared.o $(LDFLAGS) $(PCLIENT)client.o -o $(PCLIENT)client
	$(CC) $(PSERVER)shared.o $(PSERVER)inbox.o $(PSERVER)server.o -o $(PSERVER)server

client: shared.o client.o
	$(CC) $(LDFLAGS) $(PCLIENT)shared.o $(PCLIENT)client.o -o $(PCLIENT)client

client.o: $(PCLIENT)client.c $(PCLIENT)client.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(PCLIENT)client.c -o $(PCLIENT)client.o

server: inbox.o shared.o server.o
	$(CC) $(LDFLAGS) $(PSERVER)inbox.o $(PSERVER)shared.o $(PSERVER)server.o -o $(PSERVER)server

server.o: $(PSERVER)server.c $(PSERVER)server.h $(PSERVER)inbox.o
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(PSERVER)server.c -o $(PSERVER)server.o

inbox.o: $(PSERVER)inbox.c $(PSERVER)inbox.h
	$(CC) $(CFLAGS) -c $(PSERVER)inbox.c -o $(PSERVER)inbox.o

shared.o: $(PSHARED)shared.c  $(PSHARED)shared.h
	cp $(PSHARED)shared.c $(PCLIENT)shared.c
	cp $(PSHARED)shared.h $(PCLIENT)shared.h
	cp $(PSHARED)shared.c $(PSERVER)shared.c
	cp $(PSHARED)shared.h $(PSERVER)shared.h
	$(CC) $(CFLAGS) -c $(PCLIENT)shared.c -o $(PCLIENT)shared.o
	$(CC) $(CFLAGS) -c $(PSERVER)shared.c -o $(PSERVER)shared.o
	
clean:
	rm -f $(PCLIENT)shared.c $(PCLIENT)shared.h
	rm -f $(PSERVER)shared.c $(PSERVER)shared.h
	rm -f $(PCLIENT)*.o $(PCLIENT)client
	rm -f $(PSERVER)*.o $(PSERVER)server

test: all
	$(TERM) -e sh ./test.sh
	# sh ./stop.sh
	
