compile:
	gcc -o exe main.c structs.h functions.h

clean:
	rm -rf *.o
	rm -rf exe
