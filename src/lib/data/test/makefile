all: bytefifo-test bitfifo-test

bytefifo-test: bytefifo-test.cpp ../bytefifo.h ../bytefifo.o
	cd ..; make
	g++ -Wall -o bytefifo-test bytefifo-test.cpp ../bytefifo.o

bitfifo-test: bitfifo-test.cpp ../bitfifo.h ../bytefifo.h ../bitfifo.o ../bytefifo.o
	cd ..; make
	g++ -Wall -o bitfifo-test bitfifo-test.cpp ../bitfifo.o ../bytefifo.o

clean:
	rm -f *.o *~ bytefifo-test
