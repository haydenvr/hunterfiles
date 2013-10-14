#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "../cities.h"



typedef struct vNode *VList;
typedef struct QueueRep *Queue;
typedef int Item;

struct vNode { 
    Location v;     //ALICANTE etc
    Transport type; //ROAD, SEA, ANY
    VList next; 
};  

struct GraphRep { 
    int nV; // #vertices
    int nE[NUM_TRANSPORT]; //counter for number of edges of each type    
    VList connections[NUM_MAP_LOCATIONS]; // array of lists 
}; 

typedef struct QueueNode {
	Item value;
	struct QueueNode *next;
} QueueNode;

typedef struct QueueRep {
	QueueNode *head;  // ptr to first node
	QueueNode *tail;  // ptr to last node
} QueueRep;

static void makeMap(Graph g);
static void addLink(Graph g, Location start, Location end, Transport type);
static Queue newQueue(); // create new empty queue
static void dropQueue(Queue); // free memory used by queue
static void QueueJoin(Queue,Item); // add item on queue
static Item QueueLeave(Queue); // remove item from queue
static int QueueIsEmpty(Queue); // check for no items

#include "../connections_v1.2.h"

Graph newGraph() { 
    int i; 
    Graph g = malloc(sizeof(struct GraphRep));
    assert(g != NULL);
    g->nV = NUM_MAP_LOCATIONS;
    for (i = 0; i < g->nV; i++){ 
        g->connections[i] = NULL;  
    }
     
    g->nE[ROAD] = 0;
    g->nE[RAIL] = 0; 	
	g->nE[SEA] = 0;
    makeMap(g);
    return g; 
} 

void destroyGraph(Graph g){
    int i;
    VList curr;
    VList next;
    assert(g != NULL);
    assert(g->connections != NULL);
  
    for (i = 0; i < g->nV; i++){         
         curr = g->connections[i];
         while(curr !=NULL){
             next = curr->next;
             free(curr);
             curr=next;
         }
    }
    free(g);
}   


static void addLink(Graph g, Location start, Location end, Transport type){
    VList newNode = malloc(sizeof(struct vNode));
    newNode->v = end;
    newNode->type = type;
    g->nE[type]++;
    VList tmp = g->connections[start];
    if (tmp == NULL) g->connections[start] = newNode;
    else {
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = newNode;
    }
    
    newNode = malloc(sizeof(struct vNode));
    newNode->v = start;
    newNode->type = type;
    tmp = g->connections[end];
    if (tmp == NULL) g->connections[end] = newNode;
    else {
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = newNode;
    }
}

//static void makeMap(Graph g){
   
//}

//Useful for debugging
void showGraph(Graph g) { 
    assert(g != NULL); 
    printf("V=%d, E=%d + %d\n", g->nV, g->nE[ROAD],g->nE[SEA]); 
    int i; 
    for (i = 0; i < g->nV; i++) { 
        VList n = g->connections[i]; 
        while (n != NULL) { 
            printf("%d-%d ",i,n->v); 
            if(n->type == ROAD){
                printf("L ");
            } else if (n->type == RAIL) {
				printf("R ");
			}else if(n->type == SEA){
                printf("S ");
            } else {
                printf("ERROR NO SUCH TYPE");
                exit(0);
            }
            n = n->next; 
        } 
        if (g->connections [i] != NULL) 
            printf("\n"); 
        } 
} 

int numV(Graph g){
    assert(g != NULL);
    return g->nV;
}

int numE(Graph g, Transport type){
    assert(g != NULL);
    assert(type >= 0 && type <= ANY);
    if(type == ANY){
        return g->nE[ROAD] + g->nE[SEA] + g->nE[RAIL];
    } else {
        return g->nE[type];
    }
}

//returns 1 if there is an edge from start to end of the given type
//gives 0 otherwise
int isAdjacent(Graph g, Location start, Location end, Transport type){
	if (start == end) return 1;
    VList tmp = g->connections[start];
    for (; tmp != NULL; tmp = tmp->next) if (tmp->v == end && (tmp->type == type || type == ANY)) return 1;
    return 0;
}


static int dist(int st[], int e) {
    int count = 0;
    int i = e;
    while (st[i] != i) {
        count++;
        i = st[i];
    }
    return count;
}

//Determines which locations can be reached from the start location
//in n hops or less with a given transport type
//If a location can be reached in n hops or less locs[location] is set to 1
//otherwise it is left as 0
void canReachInN(Graph g, Location start, Transport type, int n, int locs[]){
    Queue q = newQueue();
    int st[NUM_MAP_LOCATIONS];
    int i, e;
    VList tmp;
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
        st[i] = -1; 
    }
    st[start] = start;
    locs[start] = 1;
    QueueJoin(q, start);
    while (!QueueIsEmpty(q)) {
        e = QueueLeave(q);
        for (tmp = g->connections[e]; tmp != NULL; tmp = tmp->next) {
            if (st[tmp->v] == -1 && dist(st, e) < n && ((tmp->type == type || type == ANY))) {
                st[tmp->v] = e;
                QueueJoin(q, tmp->v);
                locs[tmp->v] = 1;
            }  
        }
    }
    dropQueue(q);
}

//
//QUEUE IMPLEMENTATION
//

// create new empty Queue
static Queue newQueue()
{
	Queue q;
	q = malloc(sizeof(QueueRep));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Queue
static void dropQueue(Queue Q)
{
	QueueNode *curr, *next;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue rep
	free(Q);
}

// add item at end of Queue 
static void QueueJoin(Queue Q, Item it)
{
	assert(Q != NULL);
	QueueNode *new = malloc(sizeof(QueueNode));
	assert(new != NULL);
	new->value = it;
	new->next = NULL;
	if (Q->head == NULL)
		Q->head = new;
	if (Q->tail != NULL)
		Q->tail->next = new;
	Q->tail = new;
}

// remove item from front of Queue
static Item QueueLeave(Queue Q)
{
	assert(Q != NULL);
	assert(Q->head != NULL);
	Item it = Q->head->value;
	QueueNode *old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL)
		Q->tail = NULL;
	free(old);
	return it;
}

// check for no items
static int QueueIsEmpty(Queue Q)
{
	return (Q->head == NULL);
}

