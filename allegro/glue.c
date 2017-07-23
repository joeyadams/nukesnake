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

#include "glue.h"
#include "game.h"
#include "main.h"

void DrawCell(void *ctx, short x, short y, enum TileTypes icon)
{
    (void)ctx;

	y+=2;
	blit(tiles, screen, 0, (int)icon << 4, x<<4, y<<4, 16, 16);
}

//Clears the game board with floor; may be more efficient on old computers than redrawing a whole bunch of white tiles one by one
void ClearScreen(void)
{
	rectfill(screen, 0, 32, screen->w-1, screen->h-1, makecol(255,255,255));
}


static struct
{
	short dir;
	short fire;
} PlayerKeyData[2];

static short add_directions(short orig,short newkey)
{
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

//returns most recent directional indication for a given player
//used by game.c
short GetPlayerDir(short type) //PT_Right=1, PT_Left=2 (, PT_Network=3)
{
	return PlayerKeyData[type-1].dir;
}

//returns nonzero if the player pressed the fire key, zero otherwise
short GetPlayerFire(short type) //PT_Right=1, PT_Left=2 (, PT_Network=3)
{
	if (PlayerKeyData[type-1].fire==1)
	{
		PlayerKeyData[type-1].fire=2; //to keep the player from being able to hold the fire key to rapidfire
		return 1;
	}
	return 0;
}

//handles a directional keypress for a player (so that the right player's 4 and 8 will accumulate to be 7, for instance)
//used by main.c
//if diagonal_compat is set, non-diagonal keys can be combined for a diagonal result
void AcceptDirKey(short type, short direction, bool diagonals, bool diagonal_compat)
{
	if (diagonal_compat && diagonals)
		PlayerKeyData[type-1].dir = add_directions(PlayerKeyData[type-1].dir, direction);
	else if (diagonals || !dir_isdiagonal(direction))
		PlayerKeyData[type-1].dir = direction;
}

//handles a fire keypress for a player
void AcceptFireKey(short type,short val)
{
	short *fire = &PlayerKeyData[type-1].fire;
	if (!val)
		*fire = 0;
	else if (!*fire)
		*fire = 1;
}

void ClearAllKeys(void)
{
	memset(PlayerKeyData,0,sizeof(PlayerKeyData));
}

//clears the player direction key data in preparation for the next frame
//used by game.c
void ClearPlayerDirKeys(void)
{
	unsigned short i;
	for (i=0; i<sizeof(PlayerKeyData)/sizeof(*PlayerKeyData); i++)
		PlayerKeyData[i].dir=0;
}

void GlueEvent(void *ctx, enum EventTypes type, short param, short player, unsigned short x, unsigned short y)
{
    (void)ctx;
    (void)param;
    (void)player;
    (void)x;
    (void)y;

	switch (type)
	{
		case EV_Explosion:
			play_sound(S_Explosion);
			break;
		case EV_PlayerKilled:
			play_sound(S_Die);
			break;
		case EV_BulletFired:
			play_sound(S_Fire);
			break;
		case EV_RocketFired:
        case EV_NukeFired:      // TODO: Use cooler sound
			play_sound(S_Rocket);
			break;
		case EV_AmmoPickedUp:
			play_sound(S_Ammo);
			break;
		case EV_RocketPickedUp:
        case EV_NukePickedUp:   // TODO: Use cooler sound
			play_sound(S_Getrocket);
			break;
		case EV_PlayerLost:
			play_sound(S_Gameover);
			break;
	}
}

void GlueScoresChanged(void *ctx, NS *ns)
{
    (void)ctx;

	const NS_Player *left, *right;
	char buffer[256];
	left = NS_find_player_by_type(ns, PT_Left, NULL);
	if (!left)
		left = NS_find_player_by_type(ns, PT_AI, NULL);
	right = NS_find_player_by_type(ns, PT_Right, NULL);
	if (!right)
		right = NS_find_player_by_type(ns, PT_AI, left);
	if (!left || !right)
	{
		NS_log(ns, NSL_Bug, "Error finding left/right players to print score.");
		buffer[0]=0;
		goto print_buffer;
	}
	sprintf(buffer, "Red: %u/%u/%u/%u     Blue: %u/%u/%u/%u",
		left->deaths, left->bullet_count, left->rocket_count, left->nuke_count,
		right->deaths, right->bullet_count, right->rocket_count, right->nuke_count);
print_buffer:
	textout_centre_ex(screen, font, buffer, screen->w/2,20, makecol(0,0,0), makecol(255,255,255));
}

void GlueLog(void *ctx, enum NS_LogLevel logLevel, char *message)
{
    (void)ctx;

    switch (logLevel)
    {
        case NSL_Error:
            fprintf(stderr, "ERROR:  %s\n", message);
            break;
        case NSL_Glitch:
            fprintf(stderr, "GLITCH:  %s\n", message);
            break;
        case NSL_Bug:
            fprintf(stderr, "BUG:  %s\n", message);
            break;
        case NSL_Fatal:
            fprintf(stderr, "FATAL:  %s\n", message);
            break;
    }
}
