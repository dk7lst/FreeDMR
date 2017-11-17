all: parrot dmrrec

parrot: parrot.cpp ../../lib/freedmrlib.a
	g++ -Wall -lpthread -lcrypto -lssl -o parrot parrot.cpp ../../lib/freedmrlib.a

dmrrec: dmrrec.cpp ../../lib/freedmrlib.a
	g++ -Wall -lpthread -lcrypto -lssl -o dmrrec dmrrec.cpp ../../lib/freedmrlib.a

clean:
	rm -f *.o *~ parrot dmrrec
