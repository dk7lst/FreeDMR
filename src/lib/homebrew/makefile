all: homebrewclient.o homebrewpacket.o

homebrewclient.o: homebrewclient.cpp homebrewclient.h ../socket/socketlib.h
	g++ -Wall -c homebrewclient.cpp

homebrewpacket.o: homebrewpacket.cpp homebrewpacket.h
	g++ -Wall -c homebrewpacket.cpp

clean:
	rm -f *.o *~
