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

#include "ai.h"

//TODO:  Add more event calls for ammo collection (by any player), bullet fires, bullet explosions, and player deaths (as needed).

static void init_position(NS_Player *player)
{
	player->cs.ai.last_x = player->x;
	player->cs.ai.last_y = player->y;
	player->cs.ai.last_direction = player->direction;
}

void ai_init(NS_Player *player)
{
	init_position(player);
}

void ai_newround(NS_Player *player)
{
	init_position(player);
}

void ai_think(NS_Player *player)
{
	player=player; //to shut up the warnings
}

short ai_get_direction(NS_Player *player)
{
	player=player; //to shut up the warnings
	return 1+rand_ulong(8);
}

short ai_get_fire(NS_Player *player)
{
	player=player; //to shut up the warnings
	return !rand_ulong(100);
}

void ai_event(NS_Player *receiver, short type, short param, short player, unsigned short x, unsigned short y)
{
	type=param=player=x=y; //to shut up the warnings
	receiver=NULL; //ditto
}
