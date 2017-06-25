/*
 * Nuke Snake - A game where you control a nuclear-powered snake.  Your goal is
 * to destroy your opponent(s) while avoiding obstacles such as mines and rocks.
 *
 * Copyright (c) 2016 David Riggle, Joey Adams
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FRAMEDROP 10

struct NS;
struct NS_Settings;
struct NS_Player;
struct NS_Bullet;
struct NS_Effect;

typedef struct NS NS;
typedef struct NS_Settings NS_Settings;
typedef struct NS_Player NS_Player;
typedef struct NS_Bullet NS_Bullet;
typedef struct NS_Effect NS_Effect;

#include "ai.h"
#include "glue.h"
#include "menu.h"

void NS_init(NS *ns);
void NS_uninit(NS *ns);
void NS_newgame(NS *ns, unsigned short width,unsigned short height, short gametype);
void NS_newround(NS *ns, unsigned short width,unsigned short height);
short NS_frame(NS *ns); //returns one of FrameStatus
void NS_schedule_redraw();
short NS_draw(NS *ns);

NS_Player *NS_find_player_by_type(NS *ns, short type, const NS_Player *dont_pick);
	//the argument is 'dont_pick' instead of 'not' because 'not' is a C++ keyword

static inline void NS_redraw(NS *ns)
{
	NS_schedule_redraw();
	NS_draw(ns);
}

//None of the max values should be zero
#define PLAYER_MAX 2
#define BULLET_MAX 256
#define EFFECT_MAX 256

struct NS
{
	signed char *board; //board==NULL means there is no game open right now
		//The board's data consists of two parts, each width*height bytes
			//The current half is the cells the game engine will write to in order to request draws of cells
			//The actual half is what is drawn to the screen at the last redraw
		//Both board halves are used in NS_draw in order to bring the actual half up to date
	unsigned short width;
	unsigned short height;
	signed char *overlay; //This is where explosions and respawn animations are drawn
	struct NS_Settings
	{
		unsigned short
			sound:1,
			mines:1, mine_detonators:1, clusters:1, 
			bullets:1, bullet_ammo:1, rockets:1, rocket_ammo:1, nukes:1, nuke_ammo:1,
			linear_terrain:1, fractal_terrain:1, stop_at_water:1,
			diagonals:1, walls:1, rubber_walls:1;
		unsigned short tails:1, explosion_blocks:1, total_war:1;
			//total_war means there are no new rounds; players simply respawn
		unsigned char game_speed; //undefined units
		unsigned char bullet_speed; //speed in relation to player (should be 2...6)
	} settings;
	unsigned short phase; //counts down
	unsigned short paused:1, player_dying:1, reset_scheduled:1;
	struct NS_Player
	{
		char type; //Any of PlayerTypes or 0 for hole
		char direction;
		unsigned short x,y;
		signed char covering; //The tile this object replaced to be drawn at (x,y).  It will restore this tile when it moves
		char phase;
		signed char icon; //Any of TileTypes
		char alive;
		char has_tail;
		signed char fire_phase; //counts down, <=0 means the player can fire right now (minimum is -bullet_maxphase, after-fire is +bullet_maxphase)
		char trudge; //number of updates to wait before player can move
		unsigned short bullet_count; //only matters if settings.bullet_ammo==1
		unsigned short rocket_count; //only matters if settings.rocket_ammo==1
		unsigned short nuke_count; //only matters if settings.nuke_ammo==1
		unsigned short deaths;
		union
		{
			//PT_Right and PT_Left (players controlled by the keyboard) have no controller state information
			NS_AI ai; //defined in ai.h
			NS_Net net; //defined in glue.h
		} cs; //controller state
	} players[PLAYER_MAX];
	struct NS_Bullet
	{
		char type; //Any of BulletTypes or 0 for hole
		char direction;
		unsigned short x,y;
		signed char covering; //see description in NS_Player::covering
		char phase; //counts down
	} bullets[BULLET_MAX];
	struct NS_Effect
	{
		char type; //Any of EffectTypes or 0 for hole
		char frame; //counts down
		unsigned short x,y;
		char unused;
		char phase; //counts down
		short respawn;
			//players[respawn] will respawn at the end of this effect if type == ET_Respawn
			//a respawn effect will be generated at the end of this effect if type == ET_Explosion and respawn>=0
	} effects[EFFECT_MAX];
};

// TODO: Don't expose these variables in the header.

#define dir_vec(dir) (_dir_vec[(int)(dir)])
extern const signed char _dir_vec[][2];
#define vec_dir(x,y) (_vec_dir[(y)+1][(x)+1]);
extern const char _vec_dir[3][3];
static inline short average_dir(short a, short b)
{
	short ret=vec_dir((dir_vec(a)[0]+dir_vec(b)[0])/2, (dir_vec(a)[1]+dir_vec(b)[1])/2);
	if (ret<0 || ret>=8)
	{
		Bug("vec_dir returned an invalid direction %d.",ret);
		ret=0;
	}
	return ret;
}

#define bullet_icon(type,direction) (BulletH + ((type)-1)*4 + dir_hvfb[(int)(direction)])
extern const char dir_hvfb[9];

extern const char dir_opposite[9];
extern const char dir_xflip[9];
extern const char dir_yflip[9];

//evaluates 'dir' twice
#define dir_isdiagonal(dir) (dir ? ~dir&1 : 0)

enum TileTypes
{
	Floor=0,
	BluePiece, RedPiece,
	BulletH, BulletV, BulletF, BulletB,
	RocketH, RocketV, RocketF, RocketB,
	NukeH, NukeV, NukeF, NukeB,
	Explode1, Explode2,
	AmmoPack, RocketPack, NukePack,
	Mine, MineDetonator, Coal, Tree, Wall, Water, Swamp,
	BlueTail, RedTail,
	TILE_TYPE_COUNT, NullTile
};

extern const char tile_class[TILE_TYPE_COUNT];
enum TileClasses
{
	TC_Floor=0,
	TC_Player,
	TC_Bullet, //includes rockets
	TC_Explosion,
	TC_Tail,
	TC_Other
};

enum PlayerTypes
{
	PT_Right=1, PT_Left=2, PT_AI=3, PT_Network=4
};

enum BulletTypes
{
	BT_Bullet=1, BT_Rocket=2, BT_Nuke=3
};

enum EffectTypes
{
	ET_Explosion=1, ET_Respawn=2
};

enum Directions
{
	NoDirection=0, Right, DownRight, Down, DownLeft, Left, UpLeft, Up, UpRight
};

enum FrameStatus
{
	NSF_NORMAL=0, NSF_SOMEONEDIED=1
};

enum EventTypes
{
	EV_Explosion, EV_PlayerKilled,
	EV_BulletFired, EV_RocketFired, EV_NukeFired,
	EV_AmmoPickedUp, EV_RocketPickedUp, EV_NukePickedUp, EV_PlayerLost
};

enum MeansOfDeath
{
	MOD_Player, MOD_Bullet, MOD_Rocket, MOD_Nuke, MOD_Explosion, MOD_NukeRadius, MOD_Mine, MOD_Coal, MOD_Wall, MOD_Tail, MOD_ShotHimself, MOD_MineDetonator
};

#ifdef __cplusplus
}
#endif

#endif //ifndef GAME_H
