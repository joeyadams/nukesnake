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

#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

//Note that menu.c/menu.h do not handle the rendering of menus, only the platform-independent logical information
enum Commands
{
	C_None, C_NewGame, C_NewGame_1Player, C_Demo, C_NewRound, C_Pause, C_Quit,
	C_Speed1, C_Speed2, C_Speed3, C_Speed4, C_Speed5, C_Speed6, C_Speed7, C_Speed8, C_Speed9, C_Speed10,
	C_Bullet2, C_Bullet3, C_Bullet4, C_Bullet5, C_Bullet6,
	C_Sound, C_Mines, C_Bullets, C_Rockets, C_Nukes, C_BulletAmmo, C_RocketAmmo,
	C_NukeAmmo, C_SpamProtection, C_Diagonals, C_RubberWalls, C_Tails,
	C_Clusters, C_MineDetonators, C_LinearTerrain, C_FractalTerrain, C_StopAtWater,
	C_TotalWar, 
	C_COUNT
};

void NS_command(NS *ns, short command);
short NS_get_command_state(NS *ns, short command);
	#define M_Disabled 1
	#define M_Checked 2

#ifdef __cplusplus
}
#endif

#endif
