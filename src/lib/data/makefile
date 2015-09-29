all: bytefifo.o filesink.o opt.o

bytefifo.o: bytefifo.cpp bytefifo.h bytesource.h bytesink.h
	g++ -Wall -c bytefifo.cpp

filesink.o: filesink.cpp filesink.h bytesink.h
	g++ -Wall -c filesink.cpp

opt.o: opt.cpp opt.h
	g++ -Wall -c opt.cpp

clean:
	rm -f *.o *~
