#pragma once

#include "player.h"
#include "bmsresource.h"
#include "SDL/SDL_surface.h"
#include "SDL/SDL_mixer.h"

//
//
//
//
//
class BmsPlayer {
private:
	// bms data
	BmsBms *bms;
	BmsResource<Mix_Music, SDL_Surface> bmsresource;

	Player *player[4];	// maximum 4 player, theoretically.
public:
	BmsPlayer(BmsBms *bms,
		Player *p1, Player *p2, Player *p3, Player *p4);
	void Start();	// call when game start
	void Tick();	// called for each time

	// get 

	// for drawing

	// for judgement
};