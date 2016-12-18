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

unsigned long rand32(void)
{
        //randintany assumes RAND_MAX is 32767 or any greater value of 2^n-1 where n is an integer (like 65535, 131071...).
        unsigned long ret=rand();
        ret|=((unsigned long)rand())<<15;
        ret|=((unsigned long)rand())<<30;
        return ret;
}

unsigned long rand_ulong(unsigned long n)
{
        if (n<=1)
                return 0;
        return rand32()%n;
                //this does theoretically have slight preference for lower numbers
}

void DrawCell(short x, short y, short icon)
{
	if (icon<0 || icon>=TILE_TYPE_COUNT)
	{
		Bug("Icon number out of range (%d).",icon);
		return;
	}
	y+=2;
	blit(tiles,screen,0,icon<<4,x<<4,y<<4,16,16);
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
void AcceptDirKey(short type, short direction, short diagonal_compat)
{
	if (diagonal_compat && ns.settings.diagonals)
		PlayerKeyData[type-1].dir = add_directions(PlayerKeyData[type-1].dir, direction);
	else if (ns.settings.diagonals || !dir_isdiagonal(direction))
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

//used in game.c for initialization of game or round
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

void GlueEvent(short type, short param, short player, unsigned short x, unsigned short y)
{
	//to shut up the compiler warnings
	x=y;
	param=y;
	player=y;
	
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
			play_sound(S_Rocket);
			break;
		case EV_AmmoPickedUp:
			play_sound(S_Ammo);
			break;
		case EV_RocketPickedUp:
			play_sound(S_Getrocket);
			break;
		case EV_PlayerLost:
			play_sound(S_Gameover);
			break;
	}
}

void UpdateScores(void)
{
	const NS_Player *left, *right;
	char buffer[256];
	left = NS_find_player_by_type(PT_Left, NULL);
	if (!left)
		left = NS_find_player_by_type(PT_AI, NULL);
	right = NS_find_player_by_type(PT_Right, NULL);
	if (!right)
		right = NS_find_player_by_type(PT_AI, left);
	if (!left || !right)
	{
		Bug("Error finding left/right players to print score.");
		buffer[0]=0;
		goto print_buffer;
	}
	sprintf(buffer, "Red: %u/%u/%u/%u     Blue: %u/%u/%u/%u",
		left->deaths, left->bullet_count, left->rocket_count, left->nuke_count,
		right->deaths, right->bullet_count, right->rocket_count, right->nuke_count);
print_buffer:
	textout_centre_ex(screen, font, buffer, screen->w/2,20, makecol(0,0,0), makecol(255,255,255));
}
