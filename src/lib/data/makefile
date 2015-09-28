all: bytefifo.o opt.o

bytefifo.o: bytefifo.cpp bytefifo.h
	g++ -Wall -c bytefifo.cpp

opt.o: opt.cpp opt.h
	g++ -Wall -c opt.cpp

clean:
	rm -f *.o *~
