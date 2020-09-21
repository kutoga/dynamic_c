main: dynamicc.c main.c
	gcc -o $@ $^ -O3

