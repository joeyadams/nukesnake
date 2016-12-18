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

#include <graph.h>
#include "main.h"
#include "game.h"
#include "glue.h"
#include "menu.h"
#include <kbd.h>
#include <intr.h>
#include <compat.h>
#include <statline.h>
#include <menus.h>
#include <alloc.h>

static void *lcd_backup;
static void *game_picture_backup;
HANDLE menu;
INT_HANDLER oldInt1, oldInt5;
short old_prg_rate;
unsigned char old_prg_start;

static volatile unsigned long frame_counter=0;

DEFINE_INT_HANDLER(increment_frame_counter)
{
	frame_counter++;
}

#define NO_KEY RR_NO_KEY, RR_NO_KEY

int right_dir_keys[9][2];
int right_dir_keys_alt[9][2];
int left_dir_keys[9][2];
int function_keys[8][2];

#define key(rc_array) _keytest(rc_array[0], rc_array[1])

void init_keys(void)
{
	int init_right_dir_keys[9*2]=
		{RR_5, RR_6, RR_3, RR_2, RR_1, RR_4, RR_7, RR_8, RR_9};
	int init_right_dir_keys_alt[9*2]=
		{RR_2ND, RR_RIGHT, NO_KEY, RR_DOWN, NO_KEY, RR_LEFT, NO_KEY, RR_UP, NO_KEY};
	int init_left_dir_keys[9*2]=
		{RR_ALPHA, RR_APPS, RR_CATALOG, RR_MODE, RR_HOME, RR_DIAMOND, RR_2ND, RR_SHIFT, RR_ESC};
	int init_function_keys[8*2]=
		{RR_F1, RR_F2, RR_F3, RR_F4, RR_F5, RR_F6, RR_F7, RR_F8};
	memcpy(right_dir_keys,init_right_dir_keys,sizeof(right_dir_keys));
	memcpy(right_dir_keys_alt,init_right_dir_keys_alt,sizeof(right_dir_keys_alt));
	memcpy(left_dir_keys,init_left_dir_keys,sizeof(left_dir_keys));
	memcpy(function_keys,init_function_keys,sizeof(function_keys));
	
}

//test the key, but wait for it too
short _keytest_wait(short row, short col)
{
	if (_keytest(row, col))
	{
		while (_keytest(row, col));
		return 1;
	}
	return 0;
}

#define key_wait(rc_array) _keytest_wait(rc_array[0], rc_array[1])

//returns <0 if the quit key was pressed, 0> if a function key was pressed
short read_keys(void)
{
	int i;
	for (i=0;i<8;i++)
	{
		if (key_wait(function_keys[i]))
			return i+1;
	}
	AcceptFireKey(PT_Right, key(right_dir_keys[0]));
	AcceptFireKey(PT_Left, key(left_dir_keys[0]));
	for (i=1; i<9; i++)
	{
		if (key(right_dir_keys[i]))
		{
			AcceptDirKey(PT_Right, i, 0);
		}
		if (key(left_dir_keys[i]))
		{
			AcceptDirKey(PT_Left, i, 0);
		}
	}
	return 0;
}

static void memory_message(void)
{
	ST_helpMsg("FATAL:  Not enough RAM.  You need at least 10,000 bytes of RAM.");
}

static void intr_game(void)
{
	PRG_setRate(0);
	PRG_setStart(0xFB);
	SetIntVec(AUTO_INT_1, DUMMY_HANDLER);
	SetIntVec(AUTO_INT_5, increment_frame_counter);
}

static void intr_ams(void)
{
	PRG_setRate(old_prg_rate);
	PRG_setStart(old_prg_start);
	SetIntVec(AUTO_INT_1, oldInt1);
	SetIntVec(AUTO_INT_5, oldInt5);
}

static void finish(void)
{
	NS_uninit();
	intr_ams();
	GKeyFlush();
	memcpy(LCD_MEM, lcd_backup, LCD_SIZE);
	free(lcd_backup);
	free(game_picture_backup);
	HeapFree(menu);
}

void fatal(void)
{
	finish();
	memory_message();
	abort();
}

//returns <0 if the quit key was pressed, 0> if a function key was pressed
static short game_idle(void)
{
	unsigned long catchup;
	unsigned long fc_max = 4096/60/ns.settings.game_speed;
	do
	{
		short state = read_keys();
		if (state)
			return state;
	} while (frame_counter<fc_max);
	catchup = frame_counter/fc_max;
	frame_counter = 0;
	if (catchup > MAX_FRAMEDROP)
		catchup = MAX_FRAMEDROP;
	NS_schedule_redraw();
	while (catchup--)
		NS_frame();
	return 0;
}

//This does not handle disabled menus because they aren't supported in AMS 1.00
static void update_menu_states(HANDLE exec_handle)
{
	short command;
	short state;
	for (command=1; command<C_COUNT; command++)
	{
		state = NS_get_command_state(command);
		if (state & M_Checked)
			MenuCheck(exec_handle, command, MC_CHECK);
	}
}

void _main(void)
{
	randomize();
	
	init_keys();
	
	enum {P_File = C_COUNT, P_Speed};
	const short topmenu_count = 2; //used later to check the function key pressed
	menu = MenuNew(2, LCD_WIDTH, 18);
	MenuAddText (menu, 0, "File", P_File, DMF_TOP_SUB);
		MenuAddText (menu, P_File, "New Game", C_NewGame, DMF_CHILD);
		MenuAddText (menu, P_File, "New Round", C_NewRound, DMF_CHILD);
		MenuAddText (menu, P_File, "Quit", C_Quit, DMF_CHILD);
		
	MenuAddText (menu, 0, "Speed", P_Speed, DMF_TOP_SUB);
		//Add Speed subentries 1...9
		{
			char command_string[2]={'1',0};
			short command;
			for (command=C_Speed1; command<=C_Speed9; command++)
			{
				MenuAddText (menu, P_Speed, command_string, command, DMF_CHILD);
				command_string[0]++;
			}
		}
		//Add Speed entry 10
		MenuAddText (menu, P_Speed, "10", C_Speed10, DMF_CHILD);
	
	lcd_backup = malloc(LCD_SIZE);
	if (!lcd_backup)
	{
		HeapFree(menu);
		memory_message();
		return;
	}
	memcpy(lcd_backup, LCD_MEM, LCD_SIZE);
	game_picture_backup = malloc(LCD_SIZE);
	if (!game_picture_backup)
	{
		free(lcd_backup);
		HeapFree(menu);
		memory_message();
		return;
	}
	
	oldInt1 = GetIntVec(AUTO_INT_1);
	oldInt5 = GetIntVec(AUTO_INT_5);
	old_prg_rate = PRG_getRate();
	old_prg_start = PRG_getStart();
	
	NS_init();
	
	intr_game();
	
	NS_newgame(LCD_WIDTH/8, LCD_HEIGHT/8-1); //This accounts for a status bar at the top
	
	//clear_to_color(screen,makecol(255,255,255));
	
	//rest(1000); //let the window manager catch up (unneeded when we have a button to start)
	
	for (;;)
	{
		short state = game_idle();
		
		if (state<0) //a quit key was pressed
			break;
		if (state>0) //a menu key was pressed
		{
			HANDLE exec_handle;
			short result;
			
			memcpy(game_picture_backup, LCD_MEM, LCD_SIZE);
			intr_ams();
			exec_handle = MenuBegin(HLock(menu), 0, 0, 0);
			update_menu_states(exec_handle);
			do
			{
				if (state>topmenu_count)
					state=topmenu_count;
					//starting off the menu with a fowl key can freeze the program
					//When the game settings menu is implemented, state will default to that
				result = MenuKey(exec_handle, KEY_F1+state-1);
			} while (result == M_NOTMENUKEY);
			MenuEnd (exec_handle);
			HeapUnlock(menu);
			intr_game();
			memcpy(LCD_MEM, game_picture_backup, LCD_SIZE);
			
			if (result == C_Quit)
				break;
			NS_command(result);
		}
	}
	
	finish();
}

unsigned char icons[29][8]={
	{0,0,0,0,0,0,0,0},
	
	{0x18,0x7E,0x5A,0xFF,0xFF,0x5A,0x7E,0x18},
	{0x18,0x24,0x5A,0xBD,0xBD,0x5A,0x24,0x18},
	
	{0x0,0x0,0x0,0x7E,0x7E,0x0,0x0,0x0},
	{0x0,0x18,0x18,0x18,0x18,0x18,0x18,0x0},
	{0x0,0x6,0xE,0x1C,0x38,0x70,0x60,0x0},
	{0x0,0x60,0x70,0x38,0x1C,0xE,0x6,0x0},
	
	{0x0,0x0,0xFF,0xFF,0xFF,0xFF,0x0,0x0},
	{0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C},
	{0x7,0xF,0x1F,0x3E,0x7C,0xF8,0xF0,0xE0},
	{0xE0,0xF0,0xF8,0x7C,0x3E,0x1F,0xF,0x7},
	
	//TODO:  Nuke icons
	{0x0,0x0,0xFF,0xFF,0xFF,0xFF,0x0,0x0},
	{0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C},
	{0x7,0xF,0x1F,0x3E,0x7C,0xF8,0xF0,0xE0},
	{0xE0,0xF0,0xF8,0x7C,0x3E,0x1F,0xF,0x7},
	
	{0x8,0x30,0xE,0x20,0x55,0x28,0x3A,0x34},
	{0x8,0x36,0x7E,0x38,0x5F,0x2A,0x3A,0x34},
	
	{0x18,0x24,0x24,0x7E,0x99,0x99,0x66,0x0},
	{0x3F,0xF,0x1F,0x3F,0x7D,0xB9,0x90,0x60},
	{0x3F,0xF,0x1F,0x3F,0x7D,0xB9,0x90,0x60}, //TODO:  NukePack
	
	{0x18,0x5A,0x3C,0xE7,0xE7,0x3C,0x5A,0x18},
	{0x18,0x5A,0x3C,0xE7,0xE7,0x3C,0x5A,0x18}, //TODO:  MineDetinator
	{0x8,0x3C,0x7E,0xAB,0xF7,0xDB,0xFF,0x7E},
	{0x18,0x3C,0x18,0x7E,0x3C,0xFF,0x18,0x18},
	{0xFF,0xAB,0xD5,0xAB,0xD5,0xAB,0xD5,0xFF},
	{0x66,0x99,0x0,0x66,0x99,0x0,0x66,0x99},
	{0xAA,0x44,0x0,0x28,0x10,0x0,0xAA,0x44},
	{0,0,0,0,0,0,0,0}, //TODO: BlueTail
	{0,0,0,0,0,0,0,0} //TODO: RedTail
};
