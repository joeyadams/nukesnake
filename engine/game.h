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

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FRAMEDROP 10

// Forward declarations
struct NS;
struct NS_Settings;
struct NS_Player;
struct NS_Bullet;
struct NS_Effect;
struct NS_Random;
enum TileTypes;
enum TileClasses;
enum PlayerTypes;
enum BulletTypes;
enum EffectTypes;
enum Directions;
enum EventTypes;
enum MeansOfDeath;

typedef struct NS NS;
typedef struct NS_Settings NS_Settings;
typedef struct NS_Player NS_Player;
typedef struct NS_Bullet NS_Bullet;
typedef struct NS_Effect NS_Effect;
typedef struct NS_Random NS_Random;

void NS_init(NS *ns);   // constructor
void NS_uninit(NS *ns); // destructor
void NS_newgame(NS *ns, unsigned short width,unsigned short height, short gametype);
void NS_newround(NS *ns, unsigned short width,unsigned short height);

// Advance the state of the game by one step.
// A step is 1/(60 * game speed) seconds.
// This will do nothing if NS::paused is set.
void NS_frame(NS *ns);

// Draw any cells that changed since the last call to @NS_draw,
// or every cell if @force is true.
// Return true if any cells were drawn.
bool NS_draw(NS *ns, bool force);

NS_Player *NS_find_player_by_type(NS *ns, short type, const NS_Player *dont_pick);
	//the argument is 'dont_pick' instead of 'not' because 'not' is a C++ keyword

// Generate a random 32-bit integer using the RNG embedded in the gamestate.
// This is exposed for use by layout (map generation) and AI.
//
// NOTE: This should only be called from within NS_frame (including AI callbacks).
// In a networked game, all clients execute the game, layout, and even AI logic
// in isolation, and thus need to produce identical results for these.
uint32_t NS_rand32(NS *ns);

// Generate a random number from 0 to n-1.
// This uses NS_rand32; see note above.
//
// If n is 0, this will return 0.
uint32_t NS_random(NS *ns, uint32_t n);

// Seed the random number generator.  Note that:
//  * This is called automatically by NS_init, so it usually does not need to be called explicitly.
//  * This is provided for convenience, as the actual random seed is two 32-bit numbers.
void NS_seed(NS *ns, uint32_t n);

//None of the max values should be zero
#define PLAYER_MAX 2
#define BULLET_MAX 256
#define EFFECT_MAX 256

// Context used by the game logic.
//  - Game state
//  - Settings
//  - Signaling data
//  - Copy of the board to optimize redrawing

// Game state (plus extra info to optimize drawing).
struct NS
{
    // Callbacks implemented by the frontend.
    // Any of these callbacks may be null.
    struct NS_Glue
    {
        // Context passed to all glue callbacks.
        void *ctx;

        // Draw a cell at the specified coordinates.  (0,0) is the top-left corner.
        // This is called by NS_draw.
        void (*DrawCell)(void *ctx, short x, short y, enum TileTypes icon);

        // Called when an event occurs in the game, as a hook for sounds, player death messages, etc.
        // This is called by NS_frame.
        //
        // @param:  Event-specific parameter, e.g. enum MeansOfDeath for EV_PlayerKilled.
        // @player: Index of the player in the players array, or -1 if not applicable.
        void (*Event)(void *ctx, enum EventTypes type, short param, short player, unsigned short x, unsigned short y);
    } glue;

    // The current cells of the game, not including explosions and such.
    // If this is NULL, no game has been started yet.
	signed char *board;

    // Where explosions and respawn animations are drawn.
	signed char *overlay;

    // The cells that were shown the last time NS_draw was called.
    // This is used by NS_draw to only draw cells that changed.
    signed char *board_buffer;

	unsigned short width;
	unsigned short height;
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
		unsigned char game_speed; // Number of gamestate updates per 60th of a second (1-10, default 5)
		unsigned char bullet_speed; //speed in relation to player (should be 2...6)
	} settings;
	unsigned short phase; //counts down
	unsigned short paused:1, player_dying:1, reset_scheduled:1;

    // Set to 1 by NS_frame when any player's score or ammo count changes.
    unsigned short scores_changed:1;

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

    struct NS_Random
    {
        uint32_t z;
        uint32_t w;
    } random;
};

// TODO: Don't expose these variables in the header.

#define dir_vec(dir) (_dir_vec[(int)(dir)])
extern const signed char _dir_vec[][2];
#define vec_dir(x,y) (_vec_dir[(y)+1][(x)+1]);
extern const char _vec_dir[3][3];
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
	TILE_TYPE_COUNT,
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
