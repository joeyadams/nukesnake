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

#include "game.h"
#include "layout.h"
#include "glue.h"
#include "menu.h"

//TODO:  When placing players, make them not face dumb directions

/*** Public Structures ***/

NS ns;

const signed char _dir_vec[][2]=
{
	{0,0}, //NoDirection
	{1,0}, //Right
	{1,1}, //DownRight
	{0,1}, //Down
	{-1,1}, //DownLeft
	{-1,0}, //Left
	{-1,-1}, //Upleft
	{0,-1}, //Up
	{1,-1} //Upright
};
const char _vec_dir[3][3]=
{
	{UpLeft,Up,UpRight},
	{Left,NoDirection,Right},
	{DownLeft,Down,DownRight}
};

const char dir_hvfb[9]=
{
	0,0,3,1,2,0,3,1,2
};

const char dir_opposite[9]=
{
	0,5,6,7,8,1,2,3,4
};

const char dir_xflip[9]=
{
	0,5,4,3,2,1,8,7,6
};

const char dir_yflip[9]=
{
	0,1,8,7,6,5,4,3,2
};

const char tile_class[TILE_TYPE_COUNT]=
{
	TC_Floor,

	// BluePiece, RedPiece,
	TC_Player, TC_Player,

	// BulletH, BulletV, BulletF, BulletB,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,

	// RocketH, RocketV, RocketF, RocketB,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,

	// NukeH, NukeV, NukeF, NukeB,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,

	// Explode1, Explode2,
	TC_Explosion, TC_Explosion, //no normal tiles should be set to the explosion icons, but the overlay can (and often is)

	// AmmoPack, RocketPack, NukePack,
	TC_Other, TC_Other, TC_Other,

	// Mine, MineDetonator, Coal, Tree, Wall, Water, Swamp,
	TC_Other, TC_Other, TC_Other, TC_Other, TC_Other, TC_Other, TC_Other,

	// BlueTail, RedTail,
	TC_Tail, TC_Tail
};

/*** Miscellaneous Functions ***/

//type is one of enum EventTypes
//param specifies in greater detail what happened (e.g. the means of death of a player or the type of explosion)
//player is the ID of the player (i.e. player-ns.players, where player is of type NS_Player* )
//x and y are in case special effects surrounding events are implemented (they currently aren't used)
static void event(short type, short param, short player, unsigned short x, unsigned short y)
{
	NS_Player *p = ns.players;
	unsigned short count = PLAYER_MAX;
	
	GlueEvent(type, param, player, x, y);
	
	for (;count--;p++)
	{
		if (p->type != PT_AI)
			continue;
		ai_event(p, type, param, player, x, y);
	}
}

#define cell(x,y) (ns.board[(y)*ns.width+(x)])
#define cell_overlay(x,y) (ns.overlay[(y)*ns.width+(x)])
#define move_coord(x,y,direction) _move_coord(&(x), &(y), direction)
void _move_coord(unsigned short *xo, unsigned short *yo, short direction)
{
	short *x = (short*)xo;
	short *y = (short*)yo;
	*x += dir_vec(direction)[0];
	*y += dir_vec(direction)[1];
	if (*x < 0)
		*x += ns.width;
	else if (*x >= (short)ns.width)
		*x -= ns.width;
	if (*y < 0)
		*y += ns.height;
	else if (*y >= (short)ns.height)
		*y -= ns.height;
}

#define bullet_maxphase (60 / ns.settings.bullet_speed)
#define effect_maxphase (60 / 2)
	//Original NukeSnake's explosion time seems to be constant no matter what speed,
	//but here, we'll just make the frame speed twice as fast as the players'

unsigned long count_empty_spaces(void)
{
	const signed char *board = ns.board;
	const signed char *overlay = ns.overlay;
	unsigned long count = ns.width*ns.height;
	unsigned long ret = 0;
	for (;count--;board++,overlay++)
		if (!*board && *overlay<0)
			ret++;
	return ret;
}

//This function will return a non-empty space if no empty spaces are available, otherwise it will randomly pick a space
signed char *find_empty_space(void)
{
	signed char *board = ns.board;
	signed char *overlay = ns.overlay;
	unsigned long count = ns.width*ns.height;
	unsigned long empty_count;
	unsigned long selected;
	
	empty_count = count_empty_spaces();
	if (!empty_count)
	{
		Glitch("No empty spaces available; writing over the topleft tile.");
			//You would have to have a LOT of bullets flying around or have all the tiles filled up for this to happen.
		return board;
	}
	selected = rand_ulong(empty_count);
	
	for (;count--;board++,overlay++)
		if (!*board && *overlay<0 && !selected--)
			return board;
	
	Bug("find_empty_space loop fell through.");
	return ns.board;
}

static signed char effect_frame_icon(NS_Effect *obj)
{
	switch (obj->type)
	{
		case ET_Explosion:
			return Explode1+obj->frame;
			break;
		case ET_Respawn:
			if (obj->frame>=12)
				return (obj->frame&2) ? Explode2 : Floor;
			else
				return (obj->frame&2) ? ns.players[obj->respawn].icon : Floor;
			break;
	}
	return Floor;
}

static NS_Bullet *new_bullet(short type,unsigned short x,unsigned short y,short direction);
static NS_Effect *new_effect(short type,unsigned short x,unsigned short y);
static void update_effect(NS_Effect *obj);
static NS_Player *find_player(unsigned short x, unsigned short y, NS_Player *not);
static NS_Bullet *find_bullet(unsigned short x, unsigned short y, NS_Bullet *not);
static NS_Effect *find_effect(unsigned short x, unsigned short y, NS_Effect *not);
static void kill_player(NS_Player *player,short mod);


// 'wait' should be set to nonzero so there's a short zombie time after the player dies
//   otherwise, it should be zero
static void respawn_player(NS_Player *player, short wait)
{
	NS_Effect *e;
	
	if (player->alive)
	{
		Bug("respawn_player called while player still alive.");
		return;
	}
	
	player->direction = 1+rand_ulong(8);
	
	if (!wait)
	{
		unsigned long position = find_empty_space()-ns.board;
		player->x = position % ns.width;
		player->y = position / ns.width;
	}
	
	e = new_effect(ET_Respawn,player->x,player->y);
	if (!e)
		return;
	e->respawn = player - ns.players;
	if (!wait)
	{
		// Skip the dying part of the effect and only show the player spawning.
		e->frame-=16;
		cell_overlay(e->x, e->y) = effect_frame_icon(e);
	}
}

//makes the player appear on the board (called at the end of the respawn effect)
static void position_player(NS_Player *player, unsigned short x, unsigned short y)
{
	player->x = x;
	player->y = y;
	
	player->covering = Floor;
	player->alive = 1;
	player->has_tail = ns.settings.tails;
	player->fire_phase = -bullet_maxphase;
	player->trudge = 0;
	
	cell(player->x,player->y) = player->icon;
}

static void setup_board(unsigned short width, unsigned short height)
{
	if (!width)
		width=1;
	if (!height)
		height=1;
	if (ns.board)
	{
		free(ns.board);
		free(ns.overlay);
	}
	ns.board=malloc(width*height*2);
	if (!ns.board)
	{
		Fatal("Board size %ux%u could not be allocated.",width,height);
		exit(-1);
	}
	ns.overlay=malloc(width*height);
	if (!ns.overlay)
	{
		free(ns.board);
		Fatal("Board size %ux%u could not be allocated.",width,height);
		exit(-1);
	}
	memset(ns.board, Floor, width*height);
	memset(ns.board+width*height, Floor, width*height);
	memset(ns.overlay, -1, width*height);
	ns.width=width;
	ns.height=height;
	
	layout();
	
	{
		NS_Player *ptr = ns.players;
		unsigned short count = PLAYER_MAX;
		for (;count--;ptr++)
			if (ptr->type)
			{
				respawn_player(ptr,0);
			}
	}
	
	ClearScreen();
	NS_redraw();
}

//Explode like a mine or like a tree after being hit by a rocket, but don't fire shrapnel at -trigger_direction
static void explode(unsigned short x, unsigned short y, short trigger_direction)
{
	short dir;
	trigger_direction = dir_opposite[trigger_direction];
	new_effect(ET_Explosion, x, y);
	for (dir=1;dir<=8;dir++)
	{
		if (dir == trigger_direction)
			continue; //don't shoot a bullet back the same direction the trigger bullet came from
		/*if (!ns.settings.diagonals && ~dir&1)
			continue; //don't fire diagonal bullets in an explosion if diagonals are disabled
			Actually, the original NukeSnake would fire diagonal bullets in explosions even if diagonals were off
		*/
		new_bullet(BT_Bullet, x, y, dir);
		//new_bullet(rand_ulong(2)?BT_Bullet:BT_Rocket, x, y, dir); //Mega insanity
	}
}

//decimate whatever is in this cell, replacing it with an explosion
// (unless it's a player respawning)
static void destroy_cell(unsigned short x, unsigned short y)
{
	signed char *tile;
	short tile_type;
	NS_Player *p;
	NS_Bullet *b;
	NS_Effect *e;
	
	tile = &cell(x,y);
	tile_type = *tile;
	switch (tile_class[(int)*tile])
	{
		case TC_Player:
			p = find_player(x, y, NULL);
			if (!p)
				break;
			kill_player(p, MOD_NukeRadius);
			break;
		case TC_Bullet:
			b = find_bullet(x, y, NULL);
			if (!b)
				break;
			b->type = 0;
			break;
	}
	*tile = 0;
	
	tile = &cell_overlay(x,y);
	if (*tile>=0)
	{
		e = find_effect(x, y, NULL);
		if (e)
		{
			if (e->type == ET_Respawn)
				return;
			e->type = 0;
		}
	}
	
	switch (tile_type)
	{
		case Mine:
		case MineDetonator:
			explode(x,y,0);
			break;
		default:
			new_effect(ET_Explosion, x, y);
	}
}

//Nuke explosion (duh)
static void explode_nuclear(unsigned short x, unsigned short y)
{
	unsigned short ix,iy;
	unsigned short i,j;
	//Get the x,y coordinate to the topleft of the explosion radius
	move_coord(x,y,UpLeft);
	move_coord(x,y,UpLeft);
	
	iy=y;
	for (i=5;i-->0;)
	{
		ix=x;
		for (j=5;j-->0;)
		{
			//do not destroy the corners
			if (!((i==4 || i==0) && (j==4 || j==0)))
				destroy_cell(ix,iy);
			
			move_coord(ix, iy, Right);
		}
		move_coord(ix, iy, Down);
	}
}

//mod stands for "means of death";  it is any of enum MeansOfDeath
static void kill_player(NS_Player *player,short mod)
{
	player->deaths++;
	cell(player->x, player->y) = player->covering;
	player->alive = 0;
	UpdateScores();
	respawn_player(player, 1);
	event(EV_PlayerKilled, mod, player-ns.players, player->x, player->y);

    if (!ns.settings.total_war)
        ns.player_dying = 1;

    // Uncomment for silliness (to make players explode when they die).
    //explode(player->x, player->y, player->direction);
}

static void kill_bullet(NS_Bullet *obj)
{
	char bullet_type = obj->type;
	obj->type = 0;
	cell(obj->x, obj->y) = Floor;
	switch (bullet_type)
	{
		case BT_Nuke:
			explode_nuclear(obj->x, obj->y);
			break;
		case BT_Rocket:
			explode(obj->x, obj->y,obj->direction);
			break;
		default:
			new_effect(ET_Explosion, obj->x, obj->y);
			break;
	}
}

//takes an object that has already been moved in 'direction' and corrects it for bouncing if it ran into a wall (i.e. if cell(*x,*y)==Wall currently)
//it needs to be moved forward again afterwards
//this function returns the direction the object should have in appearance and for next iteration (will be the same as *direction except in squeezes)
static short bounce(unsigned short *x, unsigned short *y, char *direction)
{
	unsigned short oldx,oldy;
	char xwall,ywall;
	short old_direction;
	if (cell(*x,*y) != Wall)
		return *direction; //the ball did not run into a wall
	old_direction = *direction;
	oldx=*x;
	oldy=*y;
	move_coord(oldx,oldy,dir_opposite[(int) *direction]);
	xwall = (cell(*x,oldy)==Wall);
	ywall = (cell(oldx,*y)==Wall);
	if (!xwall && !ywall)
		*direction = dir_opposite[(int) *direction];
	else
	{
		if (xwall)
			*direction = dir_xflip[(int) *direction];
		if (ywall)
			*direction = dir_yflip[(int) *direction];
	}
	*x=oldx;
	*y=oldy;
	move_coord(oldx,oldy,*direction);
	if (cell(oldx,oldy)==Wall)
	{
		short postdir = *direction;
		*direction = average_dir(old_direction,*direction);
		return postdir;
	}
	return *direction;
}

static signed char tail_icon(NS_Player *obj)
{
	return obj->icon-BluePiece+BlueTail;
}

short get_player_direction(NS_Player *player)
{
	if (player->type == PT_Right || player->type == PT_Left)
		return GetPlayerDir(player->type);
	else //if (ptr->type == PT_AI)
		return ai_get_direction(player);
}

short get_player_fire(NS_Player *player)
{
	if (player->type == PT_Right || player->type == PT_Left)
		return GetPlayerFire(player->type);
	else //if (ptr->type == PT_AI)
		return ai_get_fire(player);
}


/***  Object Finding Functions ***/

static NS_Player *find_player(unsigned short x, unsigned short y, NS_Player *not)
{
	NS_Player *obj=ns.players;
	unsigned short count=PLAYER_MAX;
	
	for (;count--;obj++)
		if (obj->type && obj!=not && obj->x == x && obj->y == y)
			return obj;
	
	Glitch("Cell shows a player that doesn't exist.");
	
	return NULL;
}

static NS_Bullet *find_bullet(unsigned short x, unsigned short y, NS_Bullet *not)
{
	NS_Bullet *obj=ns.bullets;
	unsigned short count=BULLET_MAX;
	
	for (;count--;obj++)
		if (obj->type && obj!=not && obj->x == x && obj->y == y)
			return obj;
	
	Glitch("Cell shows a bullet that doesn't exist.");
	
	return NULL;
}

static NS_Effect *find_effect(unsigned short x, unsigned short y, NS_Effect *not)
{
	NS_Effect *obj=ns.effects;
	unsigned short count=EFFECT_MAX;
	
	for (;count--;obj++)
		if (obj->type && obj!=not && obj->x == x && obj->y == y)
			return obj;
	
	if (cell_overlay(x,y)>=0)
	{
		Glitch("Cell shows an effect that doesn't exist.");
	}
	
	return NULL;
}

NS_Player *NS_find_player_by_type(short type, const NS_Player *not)
{
	NS_Player *obj=ns.players;
	unsigned short count=PLAYER_MAX;
	
	for (;count--;obj++)
		if (obj->type == type && obj != not)
			return obj;
	
	return NULL;
}

/***  Object Creation Functions ***/

static NS_Player *new_player(short type, short icon)
{
	NS_Player *obj=ns.players;
	unsigned short count=PLAYER_MAX;
	
	for (;count--;obj++)
		if (!obj->type)
			break;
	if ((short)count+1==0)
		return NULL;
	
	memset(obj,0,sizeof(NS_Player));
	obj->type = type;
	obj->icon = icon;
	obj->direction = 1+rand_ulong(8);
	obj->bullet_count = 5;
	obj->rocket_count = 0;
	obj->nuke_count = 0;
	memset(&obj->cs,0,sizeof(obj->cs));
	
	if (ns.board) //if the player is created before the board is set up, we want that to work, too
		respawn_player(obj,0);
	
	if (obj->type == PT_AI)
		ai_init(obj);
	
	return obj;
}

static void update_bullet(NS_Bullet *obj);

static NS_Bullet *new_bullet(short type,unsigned short x,unsigned short y,short direction)
{
	NS_Bullet *obj=ns.bullets;
	unsigned short count=BULLET_MAX;
	
	for (;count--;obj++)
		if (!obj->type)
			break;
	if ((short)count+1==0)
		return NULL;
	
	obj->type = type;
	obj->direction = direction;
	obj->x = x;
	obj->y = y;
	obj->covering = cell(x,y);
	obj->phase=bullet_maxphase-1;
	
	update_bullet(obj);
	
	return obj;
}

static NS_Effect *new_effect(short type,unsigned short x,unsigned short y)
{
	NS_Effect *obj;
	unsigned short count;
	static const char effect_init_frame[2] = 
		{1, 27};
	
	for (obj=ns.effects,count=EFFECT_MAX; count--; obj++)
		if (!obj->type)
			break;
	if ((short)count+1==0)
	{ //we didn't find a hole for this effect
		if (type == ET_Respawn)
		{ //respawn effects absolutely have to get in, so we'll try to overwrite a non-respawn effect
			for (obj=ns.effects,count=EFFECT_MAX; count--; obj++)
				if (obj->type!=ET_Respawn)
					break;
			if ((short)count+1==0)
			{ //no non-respawn effects exist
				Bug("Sorry, you couldn't respawn because of this bug:  player count is greater than EFFECT_MAX");
					//I was verbose here because this bug will cause a player
					//  to not enter the game, a very noticeable problem.
					//If you are dealing with this problem, the quick fix is
					//  to go to game.h and increase EFFECT_MAX in game.h, then
					//  recompile the game.
					//Otherwise, try playing with fewer players concurrently
				return NULL;
			}
		}
		else
			return NULL;
	}
	
	if (obj->type)
		cell_overlay(obj->x, obj->y) = -1;

	obj->type = type;
	obj->frame = effect_init_frame[type-1];
	obj->x = x;
	obj->y = y;
	obj->phase = effect_maxphase;
	obj->respawn = -1;
	cell_overlay(x,y) = effect_frame_icon(obj);
	
	if (type == ET_Explosion)
		event(EV_Explosion, 0, -1, obj->x, obj->y);
	
	return obj;
}

static void update_player_direction(NS_Player *obj)
{
	short newdirection = get_player_direction(obj);
	if (newdirection) //this is the gliding behavior
		obj->direction = newdirection;
}


/***  Object Update Functions ***/

static void update_player(NS_Player *obj)
{
	signed char tile; //tile the player's running into right now
	short postdir;
	char old_type;
	NS_Player *p;
	NS_Bullet *b;
	
	update_player_direction(obj);
	
	if (!obj->alive)
		return;
	if (obj->trudge)
	{
		obj->trudge--;
		return;
	}
	
	if (!obj->direction)
		return; //to prevent collision with self
	
	cell(obj->x,obj->y) = ns.settings.tails ? tail_icon(obj) : obj->covering;
	postdir = obj->direction;
bounceback:
	move_coord(obj->x, obj->y, obj->direction);
	obj->covering = cell(obj->x,obj->y);
	obj->direction = postdir;
	cell(obj->x,obj->y) = obj->icon;
	
	tile = obj->covering;
	
	if (tile!=Water && ns.settings.explosion_blocks)
	{
		signed char c = cell_overlay(obj->x,obj->y);
		if (c>=0)
		{
			NS_Effect *e = find_effect(obj->x,obj->y,NULL);
			if (e && e->type == ET_Explosion)
				kill_player(obj, MOD_Explosion);
			return;
		}
	}
	
	switch (tile_class[(int)tile])
	{
		case TC_Floor:
			return;
		case TC_Player:
			p=find_player(obj->x,obj->y,obj);
			if (!p) //If this happens, it's due to a glitch in the game logic
			{
				obj->covering = Floor;
				return;
			}
			obj->covering = p->covering;
			kill_player(obj, MOD_Player);
			kill_player(p, MOD_Player);
			return;
		case TC_Bullet:
			b=find_bullet(obj->x, obj->y,NULL);
			if (!b) //If this happens, it's due to a glitch in the game logic
			{
				obj->covering = Floor;
				return;
			}
			if (b->covering == Water && ns.settings.stop_at_water)
			{ //The player should'nt be killed; it wasn't going to walk into the water
				cell(obj->x,obj->y) = obj->covering;
				move_coord(obj->x, obj->y, dir_opposite[(int)obj->direction]);
				obj->covering = cell(obj->x,obj->y);
				cell(obj->x,obj->y) = obj->icon;
				return;
			}
			obj->covering = b->covering;
			old_type = b->type;
			b->type = 0;
			if (old_type == BT_Nuke)
				explode_nuclear(obj->x, obj->y);
			else
				kill_player(obj, MOD_Bullet+old_type-1);
			return;
		case TC_Tail:
			obj->covering = Floor;
			kill_player(obj, MOD_Tail);
			return;
	}
	//tile_class == TC_Other
	switch (tile)
	{
		case AmmoPack:
			obj->covering = Floor;
			obj->bullet_count += 5;
			UpdateScores();
			event(EV_AmmoPickedUp, 0, obj-ns.players, obj->x, obj->y);
			return;
		case RocketPack:
			obj->covering = Floor;
			obj->rocket_count += 1;
			UpdateScores();
			event(EV_RocketPickedUp, 0, obj-ns.players, obj->x, obj->y);
			return;
		case NukePack:
			obj->covering = Floor;
			obj->nuke_count += 1;
			UpdateScores();
			event(EV_NukePickedUp, 0, obj-ns.players, obj->x, obj->y);
			return;
		case Mine:
		case Coal:
			obj->covering = Floor;
			kill_player(obj, MOD_Coal);
			return;
		case Tree:
		case Swamp:
			obj->trudge = 1;
			return;
		case Wall:
			if (ns.settings.rubber_walls)
			{
				cell(obj->x,obj->y) = obj->covering;
				postdir = bounce(&obj->x, &obj->y, &obj->direction);
				if (!obj->direction)
				{
					obj->covering = cell(obj->x,obj->y);
					obj->direction = postdir;
					cell(obj->x, obj->y) = obj->icon;
					return;
				}
				else goto bounceback;
			}
			else
			{
				obj->covering = Floor;
				kill_player(obj, MOD_Wall);
			}
			return;
		case Water:
			if (ns.settings.stop_at_water)
			{
				//undo the cell's movement by moving the other way
				cell(obj->x,obj->y) = obj->covering;
				move_coord(obj->x, obj->y, dir_opposite[(int)obj->direction]);
				obj->covering = cell(obj->x,obj->y);
				cell(obj->x,obj->y) = obj->icon;
			}
			else
				obj->trudge = 1;
			return;
	}
}

static void update_bullet(NS_Bullet *obj)
{
	signed char tile; //tile the player's running into right now
	char bullet_type = obj->type; //to compensate for when obj->type is set to 0
	short postdir;
	NS_Player *p;
	NS_Bullet *b;
	
	cell(obj->x,obj->y) = obj->covering;
	postdir = obj->direction;
bounceback:
	move_coord(obj->x, obj->y, obj->direction);
	obj->covering = cell(obj->x,obj->y);
	obj->direction = postdir;
	cell(obj->x,obj->y) = bullet_icon(obj->type, obj->direction);
	
	tile = obj->covering;
	
	if (ns.settings.explosion_blocks)
	{
		signed char c = cell_overlay(obj->x,obj->y);
		if (c>=0)
		{
			NS_Effect *e = find_effect(obj->x,obj->y,NULL);
			if (e && e->type == ET_Explosion)
			{
				obj->type = 0;
				cell(obj->x, obj->y) = obj->covering;
				new_effect(ET_Explosion, obj->x, obj->y);
			}
			return;
		}
	}
	
	switch (tile_class[(int)tile])
	{
		case TC_Floor:
			return;
		case TC_Player:
			p=find_player(obj->x,obj->y,NULL);
			if (!p) //If this happens, it's due to a glitch in the game logic
			{
				obj->covering = Floor;
				return;
			}
			cell(obj->x, obj->y) = tile;
			obj->type = 0;
			if (bullet_type == BT_Nuke)
				explode_nuclear(obj->x, obj->y);
			else
				kill_player(p, MOD_Bullet+bullet_type-1);
			return;
		case TC_Bullet:
			b=find_bullet(obj->x, obj->y, obj);
			if (!b) //If this happens, it's due to a glitch in the game logic
			{
				obj->covering = Floor;
				return;
			}
			b->type = 0;
			cell(obj->x, obj->y) = b->covering;
			obj->type = 0;
			new_effect(ET_Explosion, obj->x, obj->y);
			return;
		case TC_Tail:
			kill_bullet(obj);
			return;
	}
	//tile_class == TC_Other
	switch (tile)
	{
		case Swamp:
		case Water:
			return;
		case AmmoPack:
		case RocketPack:
		case Coal:
		case Tree:
			kill_bullet(obj);
			return;
		case Mine:
			obj->type = 0;
			cell(obj->x, obj->y) = Floor;
			if (bullet_type == BT_Nuke)
				explode_nuclear(obj->x, obj->y);
			else
				explode(obj->x, obj->y,obj->direction);
			return;
		case MineDetonator:
			obj->type = 0;
			cell(obj->x, obj->y) = Floor;
			explode(obj->x, obj->y,obj->direction);
			//if (bullet_type == BT_Nuke)
			//	//TODO:  Detonate all the mines
			return;
		case Wall:
			if (ns.settings.rubber_walls)
			{
				cell(obj->x,obj->y) = obj->covering;
				postdir = bounce(&obj->x, &obj->y, &obj->direction);
				if (!obj->direction)
				{
					obj->covering = cell(obj->x,obj->y);
					obj->direction = postdir;
					cell(obj->x, obj->y) = bullet_icon(obj->type, obj->direction);
					return;
				}
				else goto bounceback;
			}
			else
				kill_bullet(obj);
			return;
	}
}

static void update_effect(NS_Effect *obj)
{
	if (obj->frame-- <= 0)
	{
		cell_overlay(obj->x, obj->y) = -1;
		if (obj->type == ET_Respawn)
		{
			obj->type = 0;
			position_player(ns.players+obj->respawn, obj->x, obj->y);
		}
		obj->type = 0;
	}
	else
	{
		if (obj->type == ET_Respawn && obj->frame == 11)
		{
			if (ns.settings.total_war)
			{
				// Move the respawn effect to the player's new position.
				cell_overlay(obj->x, obj->y) = -1; //the effect is moving, so we need to make this spot transparent
				unsigned long position = find_empty_space()-ns.board;
				obj->x = position % ns.width;
				obj->y = position / ns.width;
			}
			else
			{
				// Schedule a reset (don't reset now while we're looping through stuff).
				ns.reset_scheduled = 1;
			}
		}
		cell_overlay(obj->x, obj->y) = effect_frame_icon(obj);
	}
}

/***  Bulk Object Update Functions ***/

static short update_players(void)
{
	NS_Player *ptr = ns.players;
	unsigned short count = PLAYER_MAX;
	char something_updated = 0;
	
	for (;count--;ptr++)
	{
		if (ptr->type)
		{
			update_player(ptr); //if the player is dead, this function will handle that
			something_updated=1;
		}
	}
	
	return something_updated;
}

static short update_bullets(void)
{
	NS_Bullet *ptr = ns.bullets;
	unsigned short count = BULLET_MAX;
	char something_updated = 0;
	
	for (;count--;ptr++)
	{
		if (!ptr->type)
			continue;
		if (ptr->phase--==0)
		{
			update_bullet(ptr);
			ptr->phase=bullet_maxphase-1;
			something_updated=1;
		}
	}
	
	return something_updated;
}

static short update_effects(void)
{
	NS_Effect *ptr = ns.effects;
	unsigned short count = EFFECT_MAX;
	char something_updated = 0;
	
	for (;count--;ptr++)
	{
		if (!ptr->type)
			continue;
		if (ptr->phase--==0)
		{
			update_effect(ptr);
			ptr->phase=effect_maxphase;
			something_updated=1;
		}
	}
	
	return something_updated;
}

static short check_fires(void)
{
	NS_Player *ptr = ns.players;
	NS_Bullet *b;
	unsigned short count = PLAYER_MAX;
	char something_updated = 0;
	char bullet_type;
	
	for (;count--;ptr++)
	{
		if (!ptr->type || !ptr->alive)
			continue;
		if (ptr->fire_phase--<=0)
		{
			if (ptr->fire_phase < -bullet_maxphase)
				ptr->fire_phase = -bullet_maxphase;
			if (get_player_fire(ptr))
			//if ((ns.phase == 0 || ns.phase == 39 )&& ptr->type == PT_Right)
			{
				unsigned short x=ptr->x, y=ptr->y;
				
				bullet_type = 0;
				
				//TODO:  implement bullet/rocket governors
				if (ns.settings.nukes && (!ns.settings.nuke_ammo || ptr->nuke_count))
				{
					bullet_type = BT_Nuke;
					if (ns.settings.nuke_ammo)
					{
						ptr->nuke_count--;
						UpdateScores();
					}
				}
				else if (ns.settings.rockets && (!ns.settings.rocket_ammo || ptr->rocket_count))
				{
					bullet_type = BT_Rocket;
					if (ns.settings.rocket_ammo)
					{
						ptr->rocket_count--;
						UpdateScores();
					}
				}
				else if (ns.settings.bullets && (!ns.settings.bullet_ammo || ptr->bullet_count))
				{
					bullet_type = BT_Bullet;
					if (ns.settings.bullet_ammo)
					{
						ptr->bullet_count--;
						UpdateScores();
					}
				}
				else
					continue;
				
				
				if (!bullet_type)
				{
					Glitch("Error determining fired bullet type.");
					continue;
				}
				
				//We know for certain now that the player can indeed fire a bullet
				//By this point, bullet_type is a valid BT_*
				something_updated=1;
				ptr->fire_phase = bullet_maxphase-1;
				update_player_direction(ptr); //so the player can fire the direction he wanted to most recently
				
				move_coord(x,y,ptr->direction);
				if (ns.settings.rubber_walls && cell(x,y)==Wall)
				{ //the player lodged between two walls tried to fire; this would cause a glitch without this segment of code that simply kills the player
					event(bullet_type+EV_BulletFired-BT_Bullet, 0, ptr - ns.players, ptr->x, ptr->y);
					kill_player(ptr, MOD_ShotHimself);
					continue;
				}
				
				b = new_bullet(bullet_type, ptr->x, ptr->y, ptr->direction);
				if (b && b->type)
					update_bullet(b);
				
				event(bullet_type+EV_BulletFired-BT_Bullet, 0, ptr - ns.players, ptr->x, ptr->y);
			}
		}
	}
	
	return something_updated;
}

//when the players move, that may keep them from being able to fire (because they'll end up shooting their own bullet)
static void players_moved_dont_fire(void)
{
	NS_Player *ptr = ns.players;
	unsigned short count = PLAYER_MAX;
	for (;count--;ptr++)
	{
		if (!ptr->type || !ptr->alive)
			continue;
		if (ptr->fire_phase > -bullet_maxphase)
			ptr->fire_phase += bullet_maxphase;
	}
}

/***  Object Other Functions ***/

static void clear_objects(void)
{
	memset(ns.players,0,sizeof(ns.players));
	memset(ns.bullets,0,sizeof(ns.bullets));
	memset(ns.effects,0,sizeof(ns.effects));
}

/*** Public Functions ***/

static const NS_Settings defaultSettings=
{
//1-bit flags:
	sound:             1,
	mines:	           1,
	mine_detonators:   1,
	clusters:          1,
	bullets:           1,
	bullet_ammo:       0,
	rockets:           1,
	rocket_ammo:       1,
	nukes:             1,
	nuke_ammo:         1,
	linear_terrain:    1,
	fractal_terrain:   1,
	stop_at_water:     1,
	diagonals:         1,
	walls:             1,
	rubber_walls:      0,
	tails:             0,
	explosion_blocks:  0,
	total_war:         0,
//Speeds:
	game_speed:        5,
	#if COMPAT_SMALL_DEVICE==0
	bullet_speed:      6
	#else
	bullet_speed:      3 //Fast bullets don't look great on such a small screen
	#endif
};

static short redraw_scheduled;

void NS_init(void)
{
	ns.board=NULL;
	ns.overlay=NULL;
	ns.settings = defaultSettings;
	redraw_scheduled = 1;
}

void NS_uninit(void)
{
	if (ns.board)
		free(ns.board);
	if (ns.overlay)
		free(ns.overlay);
	ns.board=NULL;
	ns.overlay=NULL;
}

//gametype is one of the appropriate menu items (i.e. one of
//	C_NewGame, C_NewGame_1Player, C_Demo)
void NS_newgame(unsigned short width,unsigned short height, short gametype)
{
	ClearAllKeys();
	ns.phase=59;
	ns.paused=0;
	ns.player_dying=0;
	clear_objects();
	setup_board(width,height);
	switch (gametype)
	{
		case C_NewGame: //2 player
			new_player(PT_Right, BluePiece);
			new_player(PT_Left, RedPiece);
			break;
		case C_NewGame_1Player: //1 player, 1 AI
			new_player(PT_Right, BluePiece);
			new_player(PT_AI, RedPiece);
			break;
		case C_Demo: //2 AIs
			new_player(PT_AI, BluePiece);
			new_player(PT_AI, RedPiece);
			break;
	}
	{
		//TODO:  Use an event call for this (maybe)
		NS_Player *p = ns.players;
		unsigned short count = PLAYER_MAX;
		for (;count--;p++)
		{
			if (p->type == PT_AI)
				ai_newround(p);
		}
	}
	UpdateScores();
	NS_redraw();
}

void NS_newround(unsigned short width,unsigned short height)
{
	ClearAllKeys();
	ns.phase=59;
	ns.paused=0;
	ns.reset_scheduled=0;
	ns.player_dying=0;
	
	{
		NS_Player *p = ns.players;
		unsigned short count = PLAYER_MAX;
		for (;count--;p++)
			p->alive = 0;
	}
	memset(ns.bullets,0,sizeof(ns.bullets));
	memset(ns.effects,0,sizeof(ns.effects));
	
	setup_board(width,height);
	{
		//TODO:  Use an event call for this (maybe)
		NS_Player *p = ns.players;
		unsigned short count = PLAYER_MAX;
		for (;count--;p++)
		{
			if (p->type == PT_AI)
				ai_newround(p);
		}
	}
}

//returns one of enum FrameStatus
short NS_frame(void)
{
	char something_updated=0;
	
	if (ns.paused)
		return NSF_NORMAL;
	
	if (update_effects())
		something_updated=1;

	// Only update effects (which includes player blinking) when a player has died.
	if (!ns.player_dying)
	{
		if (update_bullets())
			something_updated=1;
		if (check_fires())
			something_updated=1;
		if (!ns.phase--)
		{
			ns.phase=59;
			players_moved_dont_fire(); //when the players move, that may keep them from being able to fire (because they'll end up shooting their own bullet)
			something_updated=1;
			update_players();
			ClearPlayerDirKeys();
		}
		{
			NS_Player *p = ns.players;
			unsigned short count = PLAYER_MAX;
			for (;count--;p++)
			{
				if (p->type == PT_AI)
					ai_think(p);
			}
		}
	}

	if (ns.reset_scheduled)
	{
		ns.reset_scheduled = 0;
		NS_newround(ns.width, ns.height);
	}

	if (something_updated)
		NS_draw();
	
	return NSF_NORMAL;
}

void NS_schedule_redraw(void)
{
	redraw_scheduled = 1;
}

//draws the tiles that were changed from last redraw
//returns nonzero if something was actually redrawn
short NS_draw(void)
{
	unsigned short x,y;
	const signed char *board = ns.board;
	const signed char *overlay = ns.overlay;
	signed char *actual = ns.board + ns.width*ns.height;
	short ret=0;
	
	if (!redraw_scheduled)
		return 0;
	
	for (y=0; y<ns.height; y++)
	for (x=0; x<ns.width;  x++, board++, overlay++, actual++)
	{
		signed char thistile = *overlay<0 ? *board : *overlay;
			//thistile = the board tile unless the corresponding overlay tile is used
		if (*actual != thistile)
		{
			*actual = thistile;
			DrawCell(x, y, thistile);
			ret=1;
		}
	}
	
	redraw_scheduled=0;
	
	return ret;
}
