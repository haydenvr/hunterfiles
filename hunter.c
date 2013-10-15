#include "game.h"
#include "HunterView.h"
#include "hunter.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void decideMove (HunterView gameState) {
	char *locations[] = {
	"AL", "AM", "AT", "BA", "BI", "BE", "BR", "BO", "BU", "BC", 
	"BD", "CA", "CG", "CD", "CF", "CO", "CN", "DU", "ED", "FL",
	"FR", "GA", "GW", "GE", "GO", "GR", "HA", "JM", "KL", "LE",
	"LI", "LS", "LV", "LO", "MA", "MN", "MR", "MI", "MU", "NA",
	"NP", "NU", "PA", "PL", "PR", "RO", "SA", "SN", "SR", "SJ",
	"SO", "ST", "SW", "SZ", "TO", "VA", "VR", "VE", "VI", "ZA",
	"ZU", "NS", "EC", "IS", "AO", "BB", "MS", "TS", "IO", "AS", 
	"BS", "C?", "S?", "HI", "D1", "D2", "D3", "D4", "D5", "TP"
	};
	int round = getRound(gameState), amtLocs;
	PlayerID id = getCurrentPlayer(gameState);
    LocationID move;
	srand (time(NULL));
	if (id == PLAYER_LORD_GODALMING ) {
        move = GALWAY;
	} else if (id == PLAYER_DR_SEWARD) {
        if (round == 0) move = GALATZ;
        else if (getLocation(gameState, id) == GALATZ) move = KLAUSENBURG;
        else move = GALATZ;
	} else if (id == PLAYER_VAN_HELSING) {
		if (round == 0) move = ATHENS;
		else {
			LocationID *adj = connectedLocations(gameState, &amtLocs, getLocation(gameState,id), id, round, 1, 0, 0);
			move = adj[rand() % amtLocs];
		}
	} else if (id == PLAYER_MINA_HARKER) {
		if (round == 0) move = MADRID;
		else {
			LocationID *adj = connectedLocations(gameState, &amtLocs, getLocation(gameState,id), id, round, 1, 0, 1);
			move = adj[rand() % amtLocs];
		}
	}
 	printf("i'm going to %d\n",move);
	registerBestPlay(locations[move], "Penis <3" );
}

