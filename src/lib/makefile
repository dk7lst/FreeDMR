all:
	cd platform; make
	cd data; make
	#cd socket; make
	cd serial; make
	cd dv4mini; make
	ar rs freedmrlib.a data/*.o serial/*.o dv4mini/*.o

clean:
	cd platform; make clean
	cd data; make clean
	cd socket; make clean
	cd serial; make clean
	cd dv4mini; make clean
	rm -f *.a
