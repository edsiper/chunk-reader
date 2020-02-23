all:
	gcc -g -Wall -DMPACK_EXTENSIONS=1 mpack.c reader.c -o reader

clean:
	rm -rf *.o *~
