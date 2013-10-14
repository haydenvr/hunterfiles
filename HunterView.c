#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "game.h"
#include "HunterView.h"
#include "graph/Graph.h"

#define CHARS_PER_ROUND 40
#define CHARS_PER_TURN  8

typedef struct player *Player;
     
struct hunterView {
	int score;
    Round round;
    PlayerID currentPlayer;
    Player players[NUM_PLAYERS];
    playerMessage messages[MESSAGE_SIZE];
    int trap[TRAIL_SIZE];
    int vampire[TRAIL_SIZE];
};

struct player {
    int health;
    LocationID location[TRAIL_SIZE];
    int hospital;
};

static int isAtSea(LocationID location, HunterView hunterView);
static int isInCity(LocationID location, HunterView hunterView);
static int doubledBack (LocationID location, HunterView hunterView);

// newHunterView creates a new hunter view to summarise the current state of
// the game.
//
// pastPlays is a string of all the plays made in the game so far by all
// players (including Dracula) from earliest to most recent.
//
//
// messages is an array containing a playerMessage for each play in the game
// so far. It will have exactly the same number of elements as there are plays
// in pastPlays. The message from the first play will be at index 0, and so on.
// The contents of each playerMessage will be exactly as provided by the player.
//
// The "playerMessage" type is defined in game.h.
// You are free to ignore messages if you wish.
HunterView newHunterView( char *pastPlays, playerMessage messages[] ) {
    HunterView hunterView = malloc(sizeof(struct hunterView));
	int i, j;
	int player;
	hunterView->score = GAME_START_SCORE;
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
	for (i = 0; i < NUM_PLAYERS; i++) {
        hunterView->players[i] = malloc(sizeof(struct player));
		hunterView->players[i]->health = GAME_START_HUNTER_LIFE_POINTS;
		if (i == 4) hunterView->players[i]->health = GAME_START_BLOOD_POINTS;
        hunterView->players[i]->hospital = FALSE;
	    for (j = 0; j < TRAIL_SIZE; j++) {
            hunterView->players[i]->location[j] = -1;
		    hunterView->vampire[i] = 0;
        }
    }
	i = 0;
	while (pastPlays[i] != '\0') {
			//this character should represent the player name
			if (pastPlays[i] == 'G') player = PLAYER_LORD_GODALMING;
			else if (pastPlays[i] == 'S') player = PLAYER_DR_SEWARD;
			else if (pastPlays[i] == 'H') player = PLAYER_VAN_HELSING;
			else if (pastPlays[i] == 'M') player = PLAYER_MINA_HARKER;
			else if (pastPlays[i] == 'D') player = PLAYER_DRACULA;
			else {
				perror("Something is wrong with pastPlays\n");
				abort();
			}
			
			i++;
			int z = 0;
			
			char a[] = {pastPlays[i], pastPlays[i+1]};
			
			while (z < NUM_LOCATIONS && (locations[z][0] != a[0] || locations[z][1] != a[1])) z++;
            
            //maturing vampires
            //if (hunterView->vampire[TRAIL_SIZE - 1]) hunterView->score -= SCORE_LOSS_VAMPIRE_MATURES;
            
			//move array along 1
		    for (j = TRAIL_SIZE - 1; j > 0; j--) {
		        hunterView->players[player]->location[j] = hunterView->players[player]->location[j-1];
                hunterView->vampire[j] = hunterView->vampire[j-1];
                hunterView->trap[j] = hunterView->trap[j-1];
            }
            
            //set location array
		    hunterView->players[player]->location[0] = z;
            
            //remove hospital state
            if (hunterView->players[player]->hospital) hunterView->players[player]->hospital = FALSE;
            
			if ((z == hunterView->players[player]->location[1]) && 
			(player != PLAYER_DRACULA) && isInCity(player,hunterView)) {
				hunterView->players[player]->health += LIFE_GAIN_REST;
				if (hunterView->players[player]->health > GAME_START_HUNTER_LIFE_POINTS) 
				    hunterView->players[player]->health = GAME_START_HUNTER_LIFE_POINTS;
			}
			if ((z == TELEPORT) && (player == PLAYER_DRACULA)) 
			    hunterView->players[player]->health += LIFE_GAIN_CASTLE_DRACULA;
			
			i += 2;
		    hunterView->vampire[0] = 0;
            hunterView->trap[0] = 0;
			if (player == PLAYER_DRACULA) {
					if (pastPlays[i] == 'T') {
						hunterView->trap[0] = 1;
					}
					i++;
					if (pastPlays[i] == 'V') {
                        //vampire placed
						hunterView->vampire[0] = 1;
						
					}
					i++;
					if (pastPlays[i] == 'M') {
						//trap has left trail (already handled?)
					} else if (pastPlays[i] == 'V') {
                        //vampire matures
						hunterView->score -= SCORE_LOSS_VAMPIRE_MATURES;
					}
					i++;
					if (isAtSea(hunterView->players[player]->location[0], hunterView) || 
					isAtSea(hunterView->players[player]->
					location[doubledBack (hunterView->players[player]->location[0], hunterView)], hunterView)) 
					    hunterView->players[player]->health -= LIFE_LOSS_SEA;
					hunterView->score -= SCORE_LOSS_DRACULA_TURN;
			} else {
				int l = 0;
				while (l < 3) {
					if (pastPlays[i] == 'T') {
						hunterView->players[player]->health -= LIFE_LOSS_TRAP_ENCOUNTER;
					} else if (pastPlays[i] == 'V') {
						//immature vampire encountered (slay vampire)
					} else if (pastPlays[i] == 'D') {
						hunterView->players[PLAYER_DRACULA]->health -= LIFE_LOSS_HUNTER_ENCOUNTER;
						hunterView->players[player]->health -= LIFE_LOSS_DRACULA_ENCOUNTER;
						if (hunterView->players[PLAYER_DRACULA]->health < 0) hunterView->players[PLAYER_DRACULA]->health = 0;
					}
					i++;
					l++;
				}
				if (hunterView->players[player]->health <= 0) {
					//teleport to hospital
					hunterView->score -= SCORE_LOSS_HUNTER_HOSPITAL;
					hunterView->players[player]->health = GAME_START_HUNTER_LIFE_POINTS;
                    hunterView->players[player]->hospital = TRUE;
				}
			}
			i++; //to skip trailing dot
			if (pastPlays[i] == ' ') i++;
			//TODO something when run dracula out of health?
			//still need to consider locations and traps/immaturevamps
	}
	if (hunterView->score < 0) hunterView->score = 0;
    hunterView->round = (i + 1) / CHARS_PER_ROUND; //auto rounds down to remove incomplete rounds
    hunterView->currentPlayer = ((i + 1) / CHARS_PER_TURN) % NUM_PLAYERS;
	
	int amt_mess = (int) sizeof(messages)/sizeof(playerMessage);
	for (i = 0; i < amt_mess; i++) {
		for (j = 0; j < MESSAGE_SIZE-1; j++){
			if (messages[i][j] == '\0') break;
			hunterView->messages[i][j] = messages[i][j];
			
		}
	}
    return hunterView;
}
     
     
// this function frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.
void disposeHunterView( HunterView toBeDeleted ) {
    assert(toBeDeleted != NULL);
    free( toBeDeleted );
}

//Functions to return simple information about the current state of the game

//Get the current round
Round getRound (HunterView currentView) {
	return currentView->round;
}

//Get the id of current player - ie whose turn is it?
// Only returns a 'playerID' which is one of:
//   LORD_GODALMING (0): Lord Godalming's turn
//   DR_SEWARD      (1): Dr. Seward's turn
//   VAN_HELSING    (2): Van Helsing's turn
//   MINA_HARKER    (3): Mina Harker's turn
//   DRACULA        (4): Dracula's turn
PlayerID getCurrentPlayer (HunterView currentView) {
	assert(currentView->currentPlayer >= PLAYER_LORD_GODALMING && currentView->currentPlayer <= PLAYER_DRACULA);
	return currentView->currentPlayer;
}

//Get the current score
// Returns a positive integer [0...366]
int getScore(HunterView currentView) {
	assert(currentView->score >= 0);
	return currentView->score;
}

//Get the current health points for a given player
// 'player' specifies which players's life/blood points to return
//    and must be a value in the interval [0...4] (see 'player' type)
int getHealth(HunterView currentView, PlayerID player) {
	assert(player >= PLAYER_LORD_GODALMING && player <= PLAYER_DRACULA);
	return currentView->players[player]->health;
}

// Get the current location id of a given player
// May be UNKNOWN_LOCATION if the player has not had a turn yet
// (ie at the beginning of the game when the round is 0)
// otherwise for a hunter it should be an integer in the interval [0..70] 
// The given roundNumber is >= 0 and <= the current round number
// 'whichHunter' specifies which hunter's location to return
//    and must be a value in the interval [0...3] (see 'player' type)
// Or for dracula it should 
// gets the location of Dracula at the start of a particular round
// Returns an integer:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea
//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have 
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula
//   LOCATION_UNKNOWN if the round number is 0
LocationID getLocation(HunterView currentView, PlayerID player) {
    if (currentView->players[player]->hospital) return ST_JOSEPH_AND_ST_MARYS;
    return currentView->players[player]->location[0];
}

//Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns for the given player
// For dracula this may include integers:
//   in the interval [0...70] if Dracula was known to be in a city or sea
//   CITY_UNKNOWN     if Dracula was in an unknown city
//   SEA_UNKNOWN      if Dracula was in an unknown sea

//   HIDE             if Dracula was known to have made a hide move
//   DOUBLE_BACK_N    where N is [0...5], if Dracula was known to have
//                    made a double back move N positions back in the trail
//                    e.g. DOUBLE_BACK_1 is the last place place he visited
//   TELEPORT         if Dracula apparated back to Castle Dracula


// For any player if the move does not exist yet (i.e, the start of the game),
// the value should be UNKNOWN_LOCATION (-1)
// For example after 2 turns the array may have the contents
// {29, 182, -1, -1, -1, -1} 
// This would mean in the first move the player started on location 182 
// then moved to the current location of 29
void getHistory (HunterView currentView, PlayerID player,LocationID trail[TRAIL_SIZE]) {
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) trail[i] = currentView->players[player]->location[i];
}

//Functions that query the map to find information about connectivity

//This function returns an array of LocationID that represent all locations that are connected 
//to the given LocationID. 
//road, rail and sea are connections should only be considered if the road, rail, sea parameters 
//are TRUE.
//The size of the array should be stored in the variable pointed to by numLocations
//The array can be in any order but must contain unique entries
//Your function must take into account the round and player id for rail travel
//Your function must take into account that dracula can't move to the hospital or travel by rail
//but need not take into account draculas trail
//The destination 'from' should be included.
LocationID * connectedLocations(HunterView currentView, int * numLocations, LocationID from, 
                              PlayerID player, Round round, int road, int rail, int sea) {
	//this should come from graph file. 
	//this function alters the value pointed to be the pointer numLocations
    
    assert(road == FALSE || road == TRUE);
    assert(rail == FALSE || rail == TRUE);
    assert(sea == FALSE || sea == TRUE);    
    
	//conditions that need to be considered
	
	LocationID to_search = NUM_MAP_LOCATIONS;
	int moves_allowed = round % 4;
	if (!moves_allowed) rail = FALSE;
	if (player == PLAYER_DRACULA) rail = FALSE;
	if (!sea) to_search = ZURICH + 1; //Zurich is the last city

	Graph g = newGraph(); //our graph to check
	int i;
	*numLocations = 0;
	LocationID *connected;//[NUM_MAP_LOCATIONS];
	connected = malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS);
	int locationsFound[NUM_MAP_LOCATIONS]; //using this later
	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		connected[i] = -1;
		locationsFound[i] = FALSE;
	}
	i = 0;
	int found;
	while (i < to_search) {
		found = FALSE;

		if (road) {
			//don't need to check for duplicates here, all connections will be uninitialized
			if (isAdjacent(g,from, i, ROAD)) {
				connected[*numLocations] = i;
				(*numLocations)++;
				found = TRUE;
			}
		}
		if ((sea)&&(!found)) {
			if (isAdjacent(g,from, i, SEA)) {
				connected[*numLocations] = i;
				(*numLocations)++;
				found = TRUE;
			}
		}
		if (found) locationsFound[i] = TRUE; 
		i++;
	}
	
 	if (rail) { 
		//now we consider being able to move further by train
		//only do the check for the further cities if the condition of mod 4 is met
		//check places within two moves
		LocationID connected_by_rail[NUM_MAP_LOCATIONS];
		for (i = 0; i < NUM_MAP_LOCATIONS; i++) connected_by_rail[i] = FALSE;
		canReachInN(g, from, RAIL, moves_allowed, connected_by_rail);
		int j = 0;
		while (j < NUM_MAP_LOCATIONS) {
			if (!locationsFound[j]) {
				
				if (connected_by_rail[j]) {
					connected[*numLocations] = j;
					(*numLocations)++;
					locationsFound[j] = TRUE;
				}
			}
			j++;
		}	
	} 
	if (!locationsFound[from]) {
		connected[*numLocations] = from;
		(*numLocations)++; 
	}
	return connected;
}

static int isAtSea(LocationID location, HunterView hunterView) {
	return (((location >= NORTH_SEA)&&(location <= BLACK_SEA))||(location == SEA_UNKNOWN ));
}

static int isInCity(LocationID location, HunterView hunterView) {
	return (((location >= ALICANTE)&&(location <= ZURICH))||(location == CITY_UNKNOWN ));
}

static int doubledBack (LocationID location, HunterView hunterView) {
    if ((location >= DOUBLE_BACK_1)&&(location <= DOUBLE_BACK_5)) 
        return location - DOUBLE_BACK_1 + 1;
    else return FALSE;
}
