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


/*** Enumerations and basic definitions ***/
enum TileTypes {
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
enum TileClasses {
	TC_Floor=0,
	TC_Player,
	TC_Bullet, //includes rockets
	TC_Explosion,
	TC_Tail,
	TC_Other
};

enum PlayerTypes {
	PT_Keyboard=1, PT_AI=2, PT_Net=3
};

enum TankTypes {
	TT_Tank=1
};

enum BulletTypes {
	BT_Bullet=1, BT_Rocket=2, BT_Nuke=3
};

enum EffectTypes {
	ET_Explosion=1, ET_Respawn=2
};

enum KeyboardSides {
	KS_Left, KS_Right
};

enum Directions {
	NoDirection=0, Right, DownRight, Down, DownLeft, Left, UpLeft, Up, UpRight
};

//mod (means of death) is a highly technical jargon term used in the field of game programming, so learn it :)
enum MeansOfDeath {
	MOD_Player, MOD_Bullet, MOD_Rocket, MOD_Nuke, MOD_Explosion, MOD_NukeRadius, MOD_Mine, MOD_Coal, MOD_Wall, MOD_Tail, MOD_ShotHimself
};

enum MenuCommands {
	C_None = 0,
	C_NewGame, C_NewGame_1Player, C_Demo, C_NewRound, //game setup
	C_Speed, C_BulletSpeed, //speeds
	C_Bullets, C_Rockets, C_Nukes, //bullet types allowed
	C_BulletAmmo, C_RocketAmmo, C_NukeAmmo, //ammo required/placed
	C_Mines, C_Clusters, C_MineDetonators, C_LinearTerrain, C_FractalTerrain, C_StopAtWater, //map generation
	C_SpamProtection, C_Diagonals, C_RubberWalls, C_Tails, //game rules
	C_TotalWar, //miscellaneous
	C_COUNT
};

enum GameSounds {
	S_GetAmmo, S_Die, S_Explosion, S_Fire, S_Gameover, S_Gamepoint, S_Getrocket, S_Rocket,
	S_COUNT
};

/*** Vector stuff ***/

#define dir_vec(dir) (_dir_vec[(int)(dir)])
extern const signed char _dir_vec[][2];
#define vec_dir(x,y) (_vec_dir[(y)+1][(x)+1]);
extern const char _vec_dir[3][3];
static inline short average_dir(short a, short b) {
	short ret=vec_dir((dir_vec(a)[0]+dir_vec(b)[0])/2, (dir_vec(a)[1]+dir_vec(b)[1])/2);
	if (ret<0 || ret>=8) {
		//Bug("vec_dir returned an invalid direction %d.",ret);
		ret=0;
	}
	return ret;
}
static inline short add_directions(short orig,short newkey) {
	short x  = dir_vec(orig)[0];
	short y  = dir_vec(orig)[1];
	short kx = dir_vec(newkey)[0];
	short ky = dir_vec(newkey)[1];
	if (kx)
		x = kx;
	if (ky)
		y = ky;
	return vec_dir(x,y);
}
#define move_coord(x,y,direction) _move_coord(&(x), &(y), direction, ns->width, ns->height)
void _move_coord(unsigned short *xo, unsigned short *yo, short direction, short width, short height);

extern const char dir_hvfb[9];
extern const char dir_opposite[9];
extern const char dir_xflip[9];
extern const char dir_yflip[9];

//evaluates 'dir' twice
#define dir_isdiagonal(dir) (dir ? ~dir&1 : 0)


/*** Structures/Includes ***/

struct NukeSnake;
struct NS_Frontend;
struct NS_Settings;
struct NS_Player;
struct NS_Tank;
struct NS_Bullet;
struct NS_Effect;

typedef struct NukeSnake NukeSnake;
typedef struct NS_Frontend NS_Frontend;
typedef struct NS_Settings NS_Settings;
typedef struct NS_Player NS_Player;
typedef struct NS_Tank NS_Tank;
typedef struct NS_Bullet NS_Bullet;
typedef struct NS_Effect NS_Effect;

#include "ai.h"

//None of these max values should be zero
#define PLAYER_MAX 2
#define BULLET_MAX 256
#define EFFECT_MAX 256

struct NukeSnake {
	char *board; //board==NULL means there is no game open right now
		//The board's data consists of two parts, each width*height bytes
			//The current (first) half is the cells the game engine will write to in order to request draws of cells
			//The actual (second) half is what the screen is showing right now
		//Both board halves are used in NS_draw in order to bring the actual half up to date.  NS_draw is, in turn, scheduled for redraw by NS_schedule_redraw by frontends in order to prevent unnecessary redraw scans.
	unsigned short width;
	unsigned short height;
	char *overlay; //This is where explosions and respawn animations are drawn
	void *user;	//custom data to help uniquify multiple NukeSnake sessions
	struct NS_Frontend { //system-specific callback functions and flags that allow NukeSnake to use system resources
		void (*draw_cell)(NukeSnake *ns, short x, short y, short icon);
		void (*clear_screen)(NukeSnake *ns);
		void (*clear_cell_area)(NukeSnake *ns, short x, short y, unsigned short width, unsigned short height);
		
		void *(*malloc)(NukeSnake *ns, unsigned long size);
		void (*free)(NukeSnake *ns, void *ptr);
		
		void (*play_sound)(NukeSnake *ns, short sound);
		void (*print_message)(NukeSnake *ns, const char *str); //print game messages such as "H0tF0x blew himself up."
		void (*print_error)(NukeSnake *ns, const char *str);
		
		unsigned long net_data_size; //size of structure allocated to each player->net_data (will be allocated in one contiguous blob, really)
		
		unsigned short
			compat_small_device:1; //Small devices are given alternate settings to suit them better (more ammo, less density, etc)
	} frontend;
	struct NS_Settings {
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
	struct NS_Player {
		char type; //Any of PlayerTypes or 0 for hole
		char icon;
		NS_Tank *tank; //==NULL if the player is dead
		unsigned short bullet_count; //only matters if settings.bullet_ammo==1
		unsigned short rocket_count; //only matters if settings.rocket_ammo==1
		unsigned short nuke_count; //only matters if settings.nuke_ammo==1
		unsigned short deaths;
		union {
			struct {
				short side; //one of KeyboardSides if type==PT_Keyboard
				short dir;
				short fire;
			} keyboard;
			NS_AI ai; //for type==PT_AI
		};
		void *net_data; //for type==PT_Net
	} players[PLAYER_MAX];
	unsigned short phase; //counts down
	char paused;
	char redraw_scheduled;
	struct NS_Tank {
		char type; //Any of TankTypes or 0 for hole
		char direction;
		unsigned short x,y;
		char covering; //The tile this object replaced to be drawn at (x,y).  It will restore this tile when it moves
		char phase;
		NS_Player *driver;
		char has_tail;
		char fire_phase; //counts down, <=0 means the tank can fire right now (minimum is -bullet_maxphase, after-fire is +bullet_maxphase)
		char trudge; //number of updates to wait before player can move
	} tanks[PLAYER_MAX];
	struct NS_Bullet {
		char type; //Any of BulletTypes or 0 for hole
		char direction;
		unsigned short x,y;
		char covering; //see description in NS_Player::covering
		char phase; //counts down
	} bullets[BULLET_MAX];
	struct NS_Effect {
		char type; //Any of EffectTypes or 0 for hole
		char frame; //counts down
		unsigned short x,y;
		char unused;
		char phase; //counts down
		short respawn;
			//players[respawn] will respawn at the end of this effect if type == ET_Respawn
			//a respawn effect will be generated at the end of this effect if type == ET_Explosion and respawn>=0
	} effects[EFFECT_MAX];
	void *net_data_blob;
};

/*** Methods ***/

void NS_new(NukeSnake *ns, NS_Frontend *frontend);
void NS_delete(NukeSnake *ns);
int NS_setboardsize(NukeSnake *ns, unsigned short width, unsigned short height); //returns nonzero on success, zero on failure

void NS_frame(NukeSnake *ns);
void NS_schedule_redraw(NukeSnake *ns);

//draws the tiles that were changed from last redraw
//returns nonzero if something was actually redrawn
short NS_draw(NukeSnake *ns);

void NS_command(NukeSnake *ns, short command, short arg);
	//this is used to set up the board, change game speeds, and set game options
	//it's bundled into one command to make it easier to map menu actions to commands

//if diagonal_compat is set, non-diagonal keys can be combined for a diagonal result
void NS_accept_dir_key(NukeSnake *ns, short side, short direction, short diagonal_compat);
void NS_accept_fire_key(NukeSnake *ns, short side);

#endif

