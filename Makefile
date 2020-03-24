
CFLAGS= -Wall -Werror -g -O -ansi

main: main.o fsm.o player.o
	gcc -o main $(CFLAGS)  main.o fsm.o player.o

%.o : %.c
	gcc -c $<

clean:
	rm -f main *.o
