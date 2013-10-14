CC=gcc
CFLAGS=-Wall -Werror -g
OBJS=graphTest.o Graph.o Queue.o

graphTest : $(OBJS)
	$(CC) -o graphTest $(OBJS)

graphTest.o : graphTest.c Graph.h
Graph.o : Graph.c Graph.h
Queue.o : Queue.c Queue.h Item.h

clean :
	rm -f graphTest $(OBJS) core *.dSYM
