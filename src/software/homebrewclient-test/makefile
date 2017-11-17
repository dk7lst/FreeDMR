all: parrot

parrot: parrot.cpp ../../lib/freedmrlib.a
	g++ -Wall -lpthread -lcrypto -lssl -o parrot parrot.cpp ../../lib/freedmrlib.a

clean:
	rm -f *.o *~ parrot
