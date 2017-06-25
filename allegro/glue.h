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

#ifndef GLUE_H
#define GLUE_H

#define COMPAT_SMALL_DEVICE 0
	//Small devices are given alternate settings to suit them better (more ammo, less density, etc)
#define COMPAT_PC 1
	//PCs have keyboards that can be used for alt shortcuts on keyboards

#include <allegro.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

struct NS;

typedef struct NS_Net
{
	
} NS_Net;

#define Error(str,...) fprintf(stderr,"ERROR:  " str "\n", ##__VA_ARGS__ )
#define Glitch(str,...) fprintf(stderr,"GLITCH:  " str "\n", ##__VA_ARGS__ )
#define Bug(str,...) fprintf(stderr,"BUG:  " str "\n", ##__VA_ARGS__ )
#define Fatal(str,...) fprintf(stderr,"FATAL:  " str "\n", ##__VA_ARGS__ )

unsigned long rand_ulong(unsigned long n);
unsigned long rand32(void);
void DrawCell(short x,short y,short icon);
void ClearScreen(void);


//returns most recent directional indication for a given player
short GetPlayerDir(short type); //PT_Right=1, PT_Left=2 (, PT_Network=3)

//returns nonzero if the player pressed the fire key, zero otherwise
short GetPlayerFire(short type); //PT_Right=1, PT_Left=2 (, PT_Network=3)

//handles a directional keypress for a player (so that the right player's 4 and 8 will accumulate to be 7, for instance)
//used by main.c
//if diagonal_compat is set, non-diagonal keys can be combined for a diagonal result
void AcceptDirKey(short type, short direction, bool diagonals, bool diagonal_compat);

//handles a fire keypress for a player
void AcceptFireKey(short type,short val);

//used in game.c for initialization of game or round
void ClearAllKeys(void);

//clears the player direction key data in preparation for the next frame
//used by game.c
void ClearPlayerDirKeys(void);

//Reports an event to the glue.  In the future, this may include carnage reporting, for instance
//Currently, it is just for spawning sounds
void GlueEvent(short type, short param, short player, unsigned short x, unsigned short y);

void UpdateScores(struct NS *ns);

#endif //ifndef DRAW_H
