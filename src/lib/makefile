all:
	cd platform; make
	cd data; make
	cd socket; make
	cd serial; make
	cd dv4mini; make
	cd homebrew; make
	ar rs freedmrlib.a data/*.o serial/*.o socket/*.o dv4mini/*.o homebrew/*.o

toc:
	ar t freedmrlib.a

clean:
	cd platform; make clean
	cd data; make clean
	cd socket; make clean
	cd serial; make clean
	cd dv4mini; make clean
	cd homebrew; make clean
	rm -f *.a
