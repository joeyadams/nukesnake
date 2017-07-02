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

#include "menu.h"
#include "game.h"

//Note that menu.c/menu.h do not handle the rendering of menus, only the platform-independent logical information

#define toggle(v) ((v) = !(v))

void NS_command(NS *ns, short command)
{
	if (command>=C_Speed1 && command<=C_Speed10)
	{
		ns->settings.game_speed = command-C_Speed1+1;
		return;
	}
	if (command>=C_Bullet2 && command<=C_Bullet6)
	{
		ns->settings.bullet_speed = command-C_Bullet2+2;
		return;
	}
	switch (command)
	{
		case C_NewGame:
		case C_NewGame_1Player:
		case C_Demo:
			NS_newgame(ns, ns->width, ns->height, command);
			break;
		case C_NewRound:
			NS_newround(ns, ns->width, ns->height);
			break;
		case C_Pause:
			toggle(ns->paused);
			break;
		case C_Sound:
			toggle(ns->settings.sound);
			break;
		case C_Mines:
			toggle(ns->settings.mines);
			break;
		case C_MineDetonators:
			toggle(ns->settings.mine_detonators);
			break;
		case C_Bullets:
			toggle(ns->settings.bullets);
			break;
		case C_Rockets:
			toggle(ns->settings.rockets);
			break;
		case C_Nukes:
			toggle(ns->settings.nukes);
			break;
		case C_BulletAmmo:
			toggle(ns->settings.bullet_ammo);
			break;
		case C_RocketAmmo:
			toggle(ns->settings.rocket_ammo);
			break;
		case C_NukeAmmo:
			toggle(ns->settings.nuke_ammo);
			break;
		case C_SpamProtection:
			toggle(ns->settings.explosion_blocks);
			break;
		case C_Diagonals:
			toggle(ns->settings.diagonals);
			break;
		case C_RubberWalls:
			toggle(ns->settings.rubber_walls);
			break;
		case C_Tails:
			toggle(ns->settings.tails);
			break;
		case C_Clusters:
			toggle(ns->settings.clusters);
			break;
		case C_LinearTerrain:
			toggle(ns->settings.linear_terrain);
			break;
		case C_FractalTerrain:
			toggle(ns->settings.fractal_terrain);
			break;
		case C_StopAtWater:
			toggle(ns->settings.stop_at_water);
			break;
		case C_TotalWar:
			toggle(ns->settings.total_war);
			break;
	}
}

#undef toggle

short NS_get_command_state(NS *ns, short command)
{
	if (command>=C_Speed1 && command<=C_Speed10)
	{
		if (ns->settings.game_speed==command-C_Speed1+1)
			return M_Checked;
		else
			return 0;
	}
	if (command>=C_Bullet2 && command<=C_Bullet6)
	{
		if (ns->settings.bullet_speed==command-C_Bullet2+2)
			return M_Checked;
		else
			return 0;
	}
	switch (command)
	{
		case C_Pause:
			return ns->paused ? M_Checked : 0;
			break;
		case C_Sound:
			return ns->settings.sound ? M_Checked : 0;
			break;
		case C_Mines:
			return ns->settings.mines ? M_Checked : 0;
			break;
		case C_Bullets:
			return ns->settings.bullets ? M_Checked : 0;
			break;
		case C_Rockets:
			return ns->settings.rockets ? M_Checked : 0;
			break;
		case C_Nukes:
			return ns->settings.nukes ? M_Checked : 0;
			break;
		case C_BulletAmmo:
			return ns->settings.bullet_ammo ? M_Checked : 0;
			break;
		case C_RocketAmmo:
			return ns->settings.rocket_ammo ? M_Checked : 0;
			break;
		case C_NukeAmmo:
			return ns->settings.nuke_ammo ? M_Checked : 0;
			break;
		case C_SpamProtection:
			return ns->settings.explosion_blocks ? M_Checked : 0;
			break;
		case C_Diagonals:
			return ns->settings.diagonals ? M_Checked : 0;
			break;
		case C_RubberWalls:
			return ns->settings.rubber_walls ? M_Checked : 0;
			break;
		case C_Tails:
			return ns->settings.tails ? M_Checked : 0;
			break;
		case C_Clusters:
			return ns->settings.clusters ? M_Checked : 0;
			break;
		case C_MineDetonators:
			return ns->settings.mine_detonators ? M_Checked : 0;
			break;
		case C_LinearTerrain:
			return ns->settings.linear_terrain ? M_Checked : 0;
			break;
		case C_FractalTerrain:
			return ns->settings.fractal_terrain ? M_Checked : 0;
			break;
		case C_StopAtWater:
			return ns->settings.stop_at_water ? M_Checked : 0;
			break;
		case C_TotalWar:
			return ns->settings.total_war ? M_Checked : 0;
			break;
	}
	return 0;
}
