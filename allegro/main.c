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

#include "main.h"
#include "game.h"
#include "glue.h"
#include "menu.h"
#include <stdint.h>
#include <time.h>

DATAFILE *dat;
BITMAP *tiles;
short sound_tried;
SAMPLE *sounds[S_TypeCount];
static MENU menu[];

static NS ns_data;
static NS *ns = &ns_data;

static const char *sound_names[S_TypeCount]=
{
	"ammo.wav",
	"die.wav",
	"explosion.wav",
	"fire.wav",
	"gameover.wav",
	"gamepoint.wav",
	"getrocket.wav",
	"rocket.wav"
};

static void load_sounds(void)
{
	const char *prefix = "wav/";
	char filename[256];
	unsigned short i;
	unsigned short failed=0;
	sound_tried = 1;
	for (i=0;i<S_TypeCount;i++)
	{
		strcpy(filename,prefix);
		strcat(filename,sound_names[i]);
		sounds[i]=load_wav(filename);
		if (!sounds[i])
			failed++;
	}
	if (failed)
		fprintf(stderr,"Sounds could not be loaded.\n");
}

static void destroy_sounds(void)
{
	unsigned short i;
	for (i=0;i<S_TypeCount;i++)
		if (sounds[i])
		{
			destroy_sample(sounds[i]);
			sounds[i]=NULL;
		}
}

void play_sound(short s)
{
	if (s<0 || s>=S_TypeCount)
	{
		Bug("Sound index %d out of range.",s);
		return;
	}
	if (!ns->settings.sound)
		return;
	else if (!sound_tried)
		load_sounds();
	if (sounds[s])
	{
		stop_sample(sounds[s]); //for a more classic NukeSnake sound that doesn't get really loud
		play_sample(sounds[s],255,128,1000,0);
	}
}

static volatile unsigned long frame_counter = 0;
static volatile short close_button_pressed = 0;
	//This is also set to 1 any other time the program is closed

void increment_frame_counter()
{
	frame_counter++;
}
END_OF_FUNCTION(increment_frame_counter);

void close_button_handler(void)
{
	close_button_pressed = 1;
}
END_OF_FUNCTION(close_button_handler)

int right_dir_keys[9]=
	{KEY_5_PAD, KEY_6_PAD, KEY_3_PAD, KEY_2_PAD, KEY_1_PAD, KEY_4_PAD, KEY_7_PAD, KEY_8_PAD, KEY_9_PAD};
int right_dir_keys_alt[9]=
	{KEY_K, KEY_L, KEY_STOP, KEY_COMMA, KEY_M, KEY_J, KEY_U, KEY_I, KEY_O};
int left_dir_keys[9]=
	{KEY_S, KEY_D, KEY_C, KEY_X, KEY_Z, KEY_A, KEY_Q, KEY_W, KEY_E};

int speed_keys[10]=
	{KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0};

static void update_menu_states(MENU *menu);

//returns nonzero if the escape key was pressed
short read_keys(void)
{
	int i;
	if (key[KEY_ESC])
		return 1;
	AcceptFireKey(PT_Right, key[right_dir_keys[0]] || key[right_dir_keys_alt[0]]);
	AcceptFireKey(PT_Left, key[left_dir_keys[0]]);
	for (i=1; i<9; i++)
	{
		if (key[right_dir_keys[i]] || key[right_dir_keys_alt[i]])
		{
			AcceptDirKey(PT_Right, i, ns->settings.diagonals, 0);
		}
		if (key[left_dir_keys[i]])
		{
			AcceptDirKey(PT_Left, i, ns->settings.diagonals, 0);
		}
	}
	for (i=0;i<10;i++)
	{
		if (key[speed_keys[i]] && ns->settings.game_speed != i+1)
		{
			ns->settings.game_speed = i+1;
			update_menu_states(menu);
		}
	}
	return 0;
}

static void game_idle(void)
{
	unsigned long steps;

	// Wait until it's time to draw the next frame.
	// Poll game keys while we wait.
	for (;;)
	{
		read_keys();
		if (frame_counter != 0)
			break;

		// Wait 1 millisecond before polling again.
		// For 60 frames per second, we'll need to wait
		// around 16 times per frame.
		rest(1);
	}

	// Number of times to update the game state.
	// frame_counter is usually 1 here, but if the game is running slow,
	// we may have catching up to do.
	steps = frame_counter * ns->settings.game_speed;

	// Reset counter for next frame.
	frame_counter = 0;

	// Don't advance the game too far, just enough to prevent jitter without warping
	// the game if the user alt-tabs to something else.
	if (steps > MAX_FRAMEDROP)
		steps = MAX_FRAMEDROP;

	// Advance the game state.
	while (steps--)
		NS_frame(ns);

	// Draw the game state after these updates.
	NS_draw(ns, false);
}

long draw_count=0;

//This is wrapped so that pressing the close button will always close the game, even if the mouse is over the menu bar
static int d_menu_proc_wrapper(int msg, DIALOG *d, int c)
{
	if (close_button_pressed)
		return D_CLOSE;
	return d_menu_proc(msg, d, c);
}

static int d_game_proc(int msg, DIALOG *d, int c)
{
	(void)d;
	(void)c;

	if(close_button_pressed)
		return D_CLOSE;

	switch (msg)
	{
		case MSG_IDLE:
			game_idle();
			return D_O_K;
		case MSG_WANTFOCUS:
		case MSG_LOSTFOCUS:
			return D_WANTFOCUS;
	}
	return D_O_K;
}

static int do_nothing(void)
{
	return D_O_K;
}

static DIALOG game_dialog[];

int main(void)
{
	unsigned short w = 800;
	unsigned short h = 600;
	
	srand(time(NULL));
	
	sound_tried = 0;
	
	allegro_init();
	install_timer();
	install_keyboard();
	install_mouse();
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL))
		fprintf(stderr, "Sound error:  %s\n", allegro_error);
	
	set_color_depth(16);
	set_color_conversion(COLORCONV_NONE);
	
	dat=load_datafile("nukesnake.dat");
	if (!dat)
	{	
		fprintf(stderr,"Could not open nukesnake.dat\n\n");
		exit(1);
	}
	tiles=(BITMAP*)dat[0].dat;
	
	//if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, w, h, 0, 0) != 0)
	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, w, h, 0, 0) != 0)
	{
		//Shutdown Allegro
		allegro_exit();
		//Print error message
		fprintf(stderr,"Error setting graphics mode\n%s\n\n", allegro_error);
		exit(1);
	}
	
	clear_to_color(screen, makecol(255,255,255));
	
	NS_init(ns);
    ns->glue.DrawCell = DrawCell;
    ns->glue.Event = GlueEvent;
	
	if (ns->settings.sound)
		load_sounds();
	
	update_menu_states(menu);
	
	LOCK_VARIABLE(frame_counter);
	LOCK_FUNCTION(increment_frame_counter);

	// The frame counter increments every time we want to redraw.
	// We update the game state game_speed times per frame.
	install_int_ex(increment_frame_counter, TIMERS_PER_SECOND / 60);

	// do I need to lock close_button_pressed?  I don't know.
	LOCK_FUNCTION(close_button_handler);
	set_close_button_callback(close_button_handler);
	
	NS_newgame(ns, w/16, h/16-2, C_NewGame); //start with a 2 player game
	
	do_dialog(game_dialog, -1);
	
	NS_uninit(ns);
	
	unload_datafile(dat);
	destroy_sounds();

	return 0;
	
}
END_OF_MAIN()

/* Accelerated menu actions */

/*enum Commands
{
	C_None, C_NewGame, C_NewRound, C_Quit,
	C_Speed1, C_Speed2, C_Speed3
};*/

int ma_newgame(void)
{
	NS_command(ns, C_NewGame);
	return D_O_K;
}

int ma_newgame_1player(void)
{
	NS_command(ns, C_NewGame_1Player);
	return D_O_K;
}

int ma_demo(void)
{
	NS_command(ns, C_Demo);
	return D_O_K;
}

int ma_newround(void)
{
	NS_command(ns, C_NewRound);
	return D_O_K;
}

int ma_quit(void)
{
	return D_CLOSE;
}

int ma_pause(void)
{
	NS_command(ns, C_Pause);
	return D_O_K;
}

int ma_sound(void)
{
	NS_command(ns, C_Sound);
	return D_O_K;
}

/* End Menu Actions */

//set 'menu' to 'menu' (yes, same name) to check all menu items
static void update_menu_states(MENU *menu)
{
	if (!menu)
		return;
	for (;menu->text;menu++)
	{
		short game_state = NS_get_command_state(ns, (uintptr_t)menu->dp);
		int menu_state = 0;
		if (game_state & M_Disabled)
			menu_state |= D_DISABLED;
		if (game_state & M_Checked)
			menu_state |= D_SELECTED;
		menu->flags = menu_state;
		update_menu_states(menu->child);
	}
}

/* Menu structure / callback */

int menu_callback(void)
{
	//printf("Menu callback (command %d)\n",(int)active_menu->dp);
	short command = (uintptr_t)active_menu->dp;
	if (command == C_Quit)
		return D_CLOSE;
	NS_command(ns, command);
	update_menu_states(menu);
	return D_O_K;
}

#define id(n) ((void*)(short)n)

static MENU file_menu[]=
{
	{(char*)"&New Game (2 Player)\tCtrl+N", menu_callback, 0, 0, id(C_NewGame)},
	{(char*)"New &Game (1 Player)\tCtrl+G", menu_callback, 0, 0, id(C_NewGame_1Player)},
	{(char*)"&Demo\tCtrl+D", menu_callback, 0, 0, id(C_Demo)},
	{(char*)"Pause\tSpace", menu_callback, 0, 0, id(C_Pause)},
	{(char*)"New &Round\tCtrl+R", menu_callback, 0, 0, id(C_NewRound)},
	{(char*)"", 0, 0, 0, 0}, //separator
	{(char*)"&Quit\tCtrl+Q", menu_callback, 0, 0, id(C_Quit)},
	{0,0,0,0,0}
};

static MENU speed_menu[]=
{
	{(char*)"&1\t1", menu_callback, 0, 0, id(C_Speed1)},
	{(char*)"&2\t2", menu_callback, 0, 0, id(C_Speed2)},
	{(char*)"&3\t3", menu_callback, 0, 0, id(C_Speed3)},
	{(char*)"&4\t4", menu_callback, 0, 0, id(C_Speed4)},
	{(char*)"&5\t5", menu_callback, 0, 0, id(C_Speed5)},
	{(char*)"&6\t6", menu_callback, 0, 0, id(C_Speed6)},
	{(char*)"&7\t7", menu_callback, 0, 0, id(C_Speed7)},
	{(char*)"&8\t8", menu_callback, 0, 0, id(C_Speed8)},
	{(char*)"&9\t9", menu_callback, 0, 0, id(C_Speed9)},
	{(char*)"1&0\t0", menu_callback, 0, 0, id(C_Speed10)},
	{0,0,0,0,0}
};

static MENU bullet_menu[]=
{
	{(char*)"&2 x Speed", menu_callback, 0, 0, id(C_Bullet2)},
	{(char*)"&3 x Speed", menu_callback, 0, 0, id(C_Bullet3)},
	{(char*)"&4 x Speed", menu_callback, 0, 0, id(C_Bullet4)},
	{(char*)"&5 x Speed", menu_callback, 0, 0, id(C_Bullet5)},
	{(char*)"&6 x Speed", menu_callback, 0, 0, id(C_Bullet6)},
	{0,0,0,0,0}
};

//TODO:  instantly-effective settings should have accelerators
static MENU settings_menu[]=
{
	{(char*)"&Sound\tCtrl+S", menu_callback, 0, 0, id(C_Sound)},
	{(char*)"&Mines", menu_callback, 0, 0, id(C_Mines)},
	{(char*)"&Bullets", menu_callback, 0, 0, id(C_Bullets)},
	{(char*)"&Rockets", menu_callback, 0, 0, id(C_Rockets)},
	{(char*)"&Nukes", menu_callback, 0, 0, id(C_Nukes)},
	{(char*)"B&ullet Ammo", menu_callback, 0, 0, id(C_BulletAmmo)},
	{(char*)"R&ocket Ammo", menu_callback, 0, 0, id(C_RocketAmmo)},
	{(char*)"Nu&ke Ammo", menu_callback, 0, 0, id(C_NukeAmmo)},
	{(char*)"S&pam Protection\tCtrl+B", menu_callback, 0, 0, id(C_SpamProtection)},
	{(char*)"&Diagonals\tCtrl+D", menu_callback, 0, 0, id(C_Diagonals)},
	{(char*)"Rubb&er Walls\tCtrl+R", menu_callback, 0, 0, id(C_RubberWalls)},
	{(char*)"&Tails\tCtrl+T", menu_callback, 0, 0, id(C_Tails)},
	{(char*)"", 0,0,0,0},
	{(char*)"&Clusters", menu_callback, 0, 0, id(C_Clusters)},
	{(char*)"Mine Detonators", menu_callback, 0, 0, id(C_MineDetonators)},
	{(char*)"&Linear Terrain", menu_callback, 0, 0, id(C_LinearTerrain)},
	{(char*)"&Fractal Terrain", menu_callback, 0, 0, id(C_FractalTerrain)},
	{(char*)"Stop &At Water", menu_callback, 0, 0, id(C_StopAtWater)},
	{(char*)"", 0,0,0,0},
	{(char*)"Total &War", menu_callback, 0, 0, id(C_TotalWar)},
	{0,0,0,0,0}
};

static MENU menu[]=
{
	{(char*)"&File", 0, file_menu, 0, 0},
	{(char*)"&Speed", 0, speed_menu, 0, 0},
	{(char*)"&Bullet Speed", 0, bullet_menu, 0, 0},
	{(char*)"S&ettings", 0, settings_menu, 0, 0},
	{0,0,0,0,0}
};

#undef id


#define C(x) (x - 'a' + 1)

static DIALOG game_dialog[]=
{
	{d_game_proc, 0,0,0,0, 0,0, 0, 0, 0,0, 0,0,0},
	{d_menu_proc_wrapper, 0,0,0,16,0,0,0,0,0,0,menu,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, 27, 0, 0,0, do_nothing,0,0}, //make the ESC key not quit
	{d_keyboard_proc, 0,0,0,0, 0,0, C('n'), 0, 0,0, ma_newgame,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, C('g'), 0, 0,0, ma_newgame_1player,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, C('d'), 0, 0,0, ma_demo,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, C('r'), 0, 0,0, ma_newround,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, ' ', 0, 0,0, ma_pause,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, C('q'), 0, 0,0, ma_quit,0,0},
	{d_keyboard_proc, 0,0,0,0, 0,0, C('s'), 0, 0,0, ma_sound,0,0},
	//The speed keys are implemented by checking keys during the game loop (so we don't use 10 DIALOG entries and callback functions)
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

#undef C
