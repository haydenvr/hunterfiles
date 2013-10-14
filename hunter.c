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
	int round = getRound(gameState);
	PlayerID id = getCurrentPlayer(gameState);
    LocationID move;
	if (id == PLAYER_LORD_GODALMING ) {

	} else if (id == PLAYER_DR_SEWARD) {

	} else if (id == PLAYER_VAN_HELSING) {
		if (round == 0) move = ATHENS;
		else {
			
		}
	} else if (id == PLAYER_MINA_HARKER) {
		//if (round == 0) 
		move = MADRID;
	}
 
	registerBestPlay(locations[move], "I Wanna Stake You Dracula <3" );
}

