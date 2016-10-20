test:test.o
	gcc -o test test.o -levent -lczmq -lzmq
test.o:test.c
	cc -c test.c
clear:
	rm test test.o
