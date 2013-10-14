all: HunterView.h hunter.h
	gcc -g -o testHunter HunterView.c testHunter1.c hunter.c graph/Graph.c
clean:
	rm -rf testHunter
