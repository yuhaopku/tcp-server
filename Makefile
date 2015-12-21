#make

yhserver: ae.o anet.o yhserver.o
	gcc $^ -o $@

clean: 
	-rm yhserver *.o

.PHONY: clean
