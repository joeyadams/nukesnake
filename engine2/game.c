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
#include <stdlib.h>
#include <string.h>

/*** Public Structures / Vector Stuff ***/

const signed char _dir_vec[][2] = {
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
const char _vec_dir[3][3] = {
	{UpLeft,Up,UpRight},
	{Left,NoDirection,Right},
	{DownLeft,Down,DownRight}
};

const char dir_hvfb[9] = {
	0,0,3,1,2,0,3,1,2
};

const char dir_opposite[9] = {
	0,5,6,7,8,1,2,3,4
};

const char dir_xflip[9] = {
	0,5,4,3,2,1,8,7,6
};

const char dir_yflip[9] = {
	0,1,8,7,6,5,4,3,2
};

//width and height refer to the board
void _move_coord(unsigned short *xo, unsigned short *yo, short direction, short width, short height) {
	short *x = (short*)xo;
	short *y = (short*)yo;
	*x += dir_vec(direction)[0];
	*y += dir_vec(direction)[1];
	if (*x < 0)
		*x += width;
	else if (*x >= width)
		*x -= width;
	if (*y < 0)
		*y += height;
	else if (*y >= height)
		*y -= height;
}

const char tile_class[TILE_TYPE_COUNT] = {
	TC_Floor,
	TC_Player, TC_Player,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,
	TC_Bullet, TC_Bullet, TC_Bullet, TC_Bullet,
	TC_Explosion, TC_Explosion, //no normal tiles should be set to the explosion icons, but the overlay can (and often is)
	TC_Other, TC_Other, TC_Other,
	TC_Other, TC_Other, TC_Other, TC_Other, TC_Other, TC_Other, TC_Other,
	TC_Tail, TC_Tail
};

/*** Utility Functions ***/

static void copy_resize_2d(void *dest_2d, const void *src_2d, unsigned short dest_width, unsigned short dest_height, unsigned short src_width, unsigned short src_height, unsigned short element_size) {
	char *dest = (char*)dest_2d;
	char *src = (char*)src_2d;
	unsigned long dest_rowsize = dest_width*element_size;
	unsigned long src_rowsize = src_width*element_size;
	unsigned long copy_rowsize = src_rowsize;
	if (dest_height > src_height)
		dest_height = src_height;
	if (copy_rowsize > dest_rowsize)
		copy_rowsize = dest_rowsize;
	for (;dest_height--; dest+=dest_rowsize, src+=src_rowsize)
		memcpy(dest, src, src_rowsize);
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
	bullet_ammo:       1,
	rockets:           1,
	rocket_ammo:       1,
	nukes:             1,
	nuke_ammo:         0,
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

void dummy_function(void) {
	
}
void *defaultMalloc(NukeSnake *ns, unsigned long size) {
	ns=ns;
	return malloc(size);
}
void defaultFree(NukeSnake *ns, void *ptr) {
	ns=ns;
	free(ptr);
}
void defaultClearScreen(NukeSnake *ns) {
	void (*draw_cell)(NukeSnake *ns, short x, short y, short icon) = ns->frontend.draw_cell;
	short x,y;
	for (y=0; y<(short)ns->height; y++)
	for (x=0; x<(short)ns->width; x++)
		draw_cell(ns, x, y, Floor);
}
void defaultClearCellArea(NukeSnake *ns, short x, short y, unsigned short width, unsigned short height) {
	void (*draw_cell)(NukeSnake *ns, short x, short y, short icon) = ns->frontend.draw_cell;
	short xorig = x;
	unsigned short worig = width;
	for (;height--; y++)
	for (x=xorig,width=worig; width--; x++)
		draw_cell(ns, x, y, Floor);
}

void NS_new(NukeSnake *ns, NS_Frontend *frontend) {
	memset(ns, 0, sizeof(NukeSnake));
	
	if (frontend)
		ns->frontend = *frontend;
	
	if (!ns->frontend.draw_cell)
		ns->frontend.draw_cell = (void (*)(NukeSnake*,short,short,short))dummy_function;
	if (!ns->frontend.clear_screen)
		ns->frontend.clear_screen = defaultClearScreen;
	if (!ns->frontend.clear_cell_area)
		ns->frontend.clear_cell_area = defaultClearCellArea;
	
	if (!ns->frontend.malloc)
		ns->frontend.malloc = defaultMalloc;
	if (!ns->frontend.free)
		ns->frontend.free = defaultFree;
	
	if (!ns->frontend.play_sound)
		ns->frontend.play_sound = (void (*)(NukeSnake*,short))dummy_function;
	if (!ns->frontend.print_message)
		ns->frontend.print_message = (void (*)(NukeSnake*,const char *))dummy_function;
	if (!ns->frontend.print_error)
		ns->frontend.print_error = (void (*)(NukeSnake*,const char*))dummy_function;
	
	ns->settings = defaultSettings;
	ns->redraw_scheduled = 1;
}

void NS_delete(NukeSnake *ns) {
	if (ns->board)
		free(ns->board);
	if (ns->overlay)
		free(ns->overlay);
	if (ns->net_data_blob)
		free(ns->net_data_blob);
	ns->board = NULL;
	ns->overlay = NULL;
	ns->net_data_blob = NULL;
}

int NS_setboardsize(NukeSnake *ns, unsigned short width, unsigned short height) {
	char *board_new, *overlay_new;
	unsigned long size;
	
	if (!width)
		width=1;
	if (!height)
		height=1;
	if (width==ns->width && height==ns->height)
		return 1;
	size = width*height;
	
	board_new = ns->frontend.malloc(ns, size+size);
	if (!board_new)
		return 0;
	overlay_new = ns->frontend.malloc(ns, size);
	if (!overlay_new) {
		ns->frontend.free(ns, board_new);
		return 0;
	}
	
	memset(board_new, 0, size+size);
	memset(overlay_new, -1, size);
	
	copy_resize_2d(board_new, ns->board, width, height, ns->width, ns->height, 1);
	copy_resize_2d(board_new+size, ns->board+ns->width*ns->height, width, height, ns->width, ns->height, 1);
	copy_resize_2d(overlay_new, ns->overlay, width, height, ns->width, ns->height, 1);
	
	ns->frontend.free(ns, ns->board);
	ns->frontend.free(ns, ns->overlay);
	ns->board = board_new;
	ns->overlay = overlay_new;
	
	//TODO:  redraws
	
	ns->width = width;
	ns->height = height;
	
	return 1;
}

int main(int argc, char *argv[]) {
	argc = argc;
	argv[0] = argv[0];
	
	return 0;
}

