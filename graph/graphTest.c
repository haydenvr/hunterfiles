#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cities.h"
#include "Graph.h"


void showIsAdjacent(Graph g);
void showCanReach(Graph g);

int main(int argc, char * argv[]){
    
    Graph g;
    printf("Test newGraph\n");
    g = newGraph();    
    showGraph(g);
    assert(numV(g) == NUM_MAP_LOCATIONS);
    assert(numE(g,LAND) == 17);
    assert(numE(g, SEA) == 8);
    showIsAdjacent(g);
    printf("Passed\n");
    printf("Destroying graph\n");
    destroyGraph(g);
    printf("Finished destroy \n");
    return 0;
}

//Prints out adjacency information for all
//pairs of vertices for all connection types
//Can be used for debugging
void showIsAdjacent(Graph g){
    int i,j;
    printf("Sea connections\n");
    for(i=0; i < NUM_MAP_LOCATIONS; i++){
        for(j=0; j < NUM_MAP_LOCATIONS; j++){
            printf("%d ",isAdjacent(g,i,j,SEA));
        }
        printf("\n");
    }
    printf("LAND connections\n");
    for(i=0; i < NUM_MAP_LOCATIONS; i++){
        for(j=0; j < NUM_MAP_LOCATIONS; j++){
            printf("%d ",isAdjacent(g,i,j,LAND));
        }
        printf("\n");
    }
    printf("ANY connections\n");
    for(i=0; i < NUM_MAP_LOCATIONS; i++){
        for(j=0; j < NUM_MAP_LOCATIONS; j++){
            printf("%d ",isAdjacent(g,i,j,ANY));
        }
        printf("\n");
    }

}

void printLocations(Location locs[]){
    Location i;
    for(i = 0; i < NUM_MAP_LOCATIONS; i++){
        printf("%d",locs[i]);
    }
    printf("\n");
}

//prints out the results for the canReachInN function 
// with values of n from 0 to 3, from all 
//starting locations, via all transport types
void showCanReach(Graph g){
    int n, t;
    Location loc;
    for(n = 0; n <= 3; n++){
        printf("n is %d\n", n);
        for(loc = 0; loc < NUM_MAP_LOCATIONS; loc++){
            for(t = LAND; t <= ANY; t++){
                Location locations[NUM_MAP_LOCATIONS] = {0};
                canReachInN(g, loc, t, n, locations);
                printLocations(locations);
            }
        }
    }
}

