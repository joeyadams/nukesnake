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

#include "layout.h"
#include "glue.h"

//Laying out the board is given its own source file because of the complexity associated with linear and cluster (a.k.a. fractal) terrain

//A 40x30 testing board
const char *testing_board=
{
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"W       TT  AAAA                       W"
	"W            AAAA        MM            W"
	"W         AAA AAA                      W"
	"W  PPPP  AA AAAAAA        M            W"
	"W  PAPAAAA  ATTAAAA                    W"
	"W PAAAAA    AATTAAA                    W"
	"W ACAAP   CCCAAAAA                     W"
	"WPACAP   TPPAAAAAA    T        MMM     W"
	"WPAAP    T PAAAA  AA TTTT       M      W"
	"W                                      W"
	"W      R                               W"
	"W                   MMM                W"
	"W   M               MMM                W"
	"W        OOOOOOO    MMM       O        W"
	"W                            O         W"
	"W       TT  AAAA                       W"
	"W            AAAA                      W"
	"W         AAA AAA          M   M   M   W"
	"W  PPPP  AA AAAAAA                     W"
	"W  PAPAAAA  ATTAAAA                    W"
	"W PAAAAA    AATTAAA      O     M       W"
	"W ACAAP   CCCAAAAA                     W"
	"WPACAP   TPPAAAAAA    T                W"
	"WPAAP    T PAAAA  AA TTTT              W"
	"WPAAP    T PAAAA  AA TTTT              W"
	"WPAAP    T PAAAA  AA TTTT    W W       W"
	"WPAAP    T PAAAA  AA TTTT    WWW       W"
	"WPAAP    T PAAAA  AA TTTT              W"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
};

//A 50x36 testing board
const char *maze_testing_board=
{
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWPPPPPPPPPP"
	"W       TT  AAAA                       WPPPPPPPPPP"
	"W            AAAA        MM            WPPPPPPPPPP"
	"W         AAA AAA             W WW W   WPPPPPPPPPP"
	"W  PPPP  AA AAAAAA        M   W    W   WPPPPPPPPPP"
	"W  PAPAAAA  ATTAAAA           WWWWWW   WPPPPPPPPPP"
	"W PAAAAA    AATTAAA                    WPPPPPPPPPP"
	"W ACAAP   CCCAAAAA                     WPPPPPPPPPP"
	"WPACAP   TPPAAAAAA    T        MMM     WPPPPPPPPPP"
	"WPAAP    T PAAAA  AA TTTT       M      WPPPPPPPPPW"
	"W                                      WPPPPPPPPW "
	"W      R                               WPPPPPPPWP "
	"W                   MMM                WPPPPPPW P "
	"W   M               MMM                WPPPPPWP PW"
	"W        OOOOOOO    MMM       O        WPPPPW P WP"
	"W                            O         WPPPW   WPP"
	"W       TT  AAAA                       WPPW   WPPP"
	"W            AAAA                      WPPW   WPPP"
	"W         AAA AAA          M   M   M   WPPW W WPPP"
	"W  PPPP  AA AAAAAA                     WPPW W WPPP"
	"W  PAPAAAA  ATTAAAA                    WPPW W WPPP"
	"W PAAAAA    AATTAAA      O     M       W W  W WPPP"
	"W ACAAP   CCCAAAAA                     W    W WPPP"
	"WPACAP   TPPAAAAAA    T                W WW W WPPP"
	"WPAAP    T PAAAA  AA TTTT              W  W W WPPP"
	"WPAAP    T PAAAA  AA TTTT              W  W W WPPP"
	"WPAAP    T PAAAA  AA TTTT    W W       W  W W WPPP"
	"WPAAP    T PAAAA  AA TTTT    WWW       WWWW W WPPP"
	"WPAAP    T PAAAA  AA TTTT                   W WPPP"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW WPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPW        WPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPW WWWWWWWWPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPW WPPPPPPPPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPW  WWWWPPPPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPW    WPPPPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPWWWWPPPPPP"
};

//A 20x12 testing board
const char *micro_testing_board=
{
	"WWWWWWWWWWWWWWWWWWWW"
	"W       TT         W"
	"W                  W"
	"W    W    AA       W"
	"W    PP  AA    CC  W"
	"W  PAPA  A     T   W"
	"W P           M    W"
	"W A   P   CC  MM   W"
	"W   M    TTP  MM   W"
	"W    MM  TTP       W"
	"W    MM   W        W"
	"WWWWWWWWWWWWWWWWWWWW"
};

//A 20x12 testing board
const char *micro_testing_board_less=
{
	"WWWWWWWWWWWWWWWWWWWW"
	"W       TT         W"
	"W                  W"
	"W                  W"
	"W    PP   A    CC  W"
	"W  PAPA  A     T   W"
	"W             M    W"
	"W             MM   W"
	"W   M         MM   W"
	"W    MM            W"
	"W         W        W"
	"WWWWWWWWWWWWWWWWWWWW"
};

//A 20x12 testing board
const char *micro_testing_board_mines=
{
	"WWWWWWWWWWWWWWWWWWWW"
	"W                  W"
	"W M M M M M M M M  W"
	"W                  W"
	"W M M M M M M M M  W"
	"W                  W"
	"W M M M M M M M M  W"
	"W                  W"
	"W M M M M M M M M  W"
	"W                  W"
	"W M M M M M M M M  W"
	"WWWWWWWWWWWWWWWWWWWW"
};

static signed char letter_to_tile(char c)
{
	switch (c)
	{
		case ' ':
			return Floor;
		case 'O':
			return AmmoPack;
		case 'R':
			return RocketPack;
		case 'M':
			return Mine;
		case 'C':
			return Coal;
		case 'T':
			return Tree;
		case 'W':
			return Wall;
		case 'A':
			return Water;
		case 'P':
			return Swamp;
	}
	Error("Bad tile character \'%c\'",c);
	return Floor;
}

static void setup_testing_board(NS *ns, const char *init)
{
	signed char *board = ns->board;
	unsigned long count = ns->width * ns->height;
	for (;count--;init++, board++)
		*board = letter_to_tile(*init);
}

typedef struct coord
{
	unsigned short x,y;
} coord;

//This function was translated from Pascal out of David Riggle's Win Snake 2.0
//TODO:  Check division and modulus
//TODO:  Change the 'try' keyword to something else
void InitMap(NS *ns)
{
	long n,i,e,try;
	int count;
	long total_squares;
	unsigned short x, y;
	long map_index;
	signed char there; //referencing a member of ns->board
	int average, delta;
	coord cluster_loc, cluster_size;
	signed char *board = ns->board;
	
	total_squares = ns->width * ns->height;
	
	memset(board,0,total_squares);
	
	//add fractal terrain
	if (ns->settings.fractal_terrain)
	{
		//rock (coal), swamp, trees, river, blank
		//build fractal terrain map
		
		//iterate a few times with decreasing magnitudes
		#if SMALL_DEVICE==0
		delta=24;
		#else
		delta=16;
		#endif
		while (delta>0)
		{
			//perturb each square
			for (i=ns->width+1; i<total_squares-ns->width-1;i++)
			{
				//average surrounding squares
				average = board[i-ns->width-1]+board[i-ns->width]+board[i-ns->width+1]+
				          board[i-1]+board[i]+board[i+1]+
				          board[i+ns->width-1]+board[i+ns->width]+board[i+ns->width+1];
				average/=9;
				//add random delta
				if (NS_rand32(ns) & 1)
					average+=delta;
				else
					average-=delta;
				board[i]=average;
			}
			//decrease delta
			delta>>=1;
		}
		
		//set terrain based on altitude
		for (i=0;i<total_squares;i++)
		{
			signed char tile = board[i];
			#if SMALL_DEVICE==1
			tile*=2; //diversify terrain
			#endif
			if (tile<-18 || tile>=11)
				there = Coal;
			else if (tile<=-12)
				there = Water;
			else if (tile<=-8)
				there = Swamp;
			else if (tile<=4)
				there = Floor;
			else if (tile<=10)
				there = Tree;
			else
				there = Coal; //flow should never reach here
			board[i] = there;
		}
	}
	
	//add linear terrain
	if (ns->settings.linear_terrain)
	{
		//Random walk terrain
		//pick a random spot & grow a river
		for (n=0;n<2;n++) //once for trees, once for rivers
		{
			there = n?Tree:Water;
			x = (NS_random(ns, ns->width)+ns->width) / 3;
			y = (NS_random(ns, ns->height)+ns->height) / 3;
			e = NS_random(ns, total_squares);
			for (i=0;i<e;i++)
			{
				map_index = y*ns->width + x;
				board[map_index] = there;
				board[map_index+1] = there;
				switch (NS_random(ns, 4))
				{
					case 0:
						break;
					case 1:
					case 2:
						if (y<ns->height-2)
							y++;
						break;
					case 3:
						if (y>1)
							y--;
						break;
				}
				switch (NS_random(ns, 4))
				{
					case 0:
						break;
					case 1:
					case 2:
						if (x<ns->width-2)
							x++;
						break;
					case 3:
						if (x>1)
							x--;
						break;
				}
				//grow a swamp, forest, or rock garden
				if (!NS_random(ns, 25))
				{
					signed char selections[4] = {Coal, Swamp, Tree, Water};
					there = selections[NS_random(ns, 4)];
				}
			}
		}
	}
	
	//set cluster size & location
	//TODO:  Audit cluster size/location for potential buffer overruns
	if (ns->settings.clusters)
	{
		unsigned short min = ns->width;
		if (ns->height<min)
			min = ns->height;
		cluster_size.y = NS_random(ns, min);
		if (cluster_size.y == 0)
			cluster_size.y = 1;
		cluster_size.x = cluster_size.y;
		cluster_loc.y = NS_random(ns, ns->height - cluster_size.y);
		cluster_loc.x = NS_random(ns, ns->width - cluster_size.x);
	}
	else
	{
		cluster_loc.x = 0;
		cluster_loc.y = 0;
		cluster_size.x = ns->width;
		cluster_size.y = ns->height;
	}
	
	//add mines
	if (ns->settings.mines)
	{
		#if SMALL_DEVICE==0
		//randomly place a random number of mines on blank ground (2% max)
		count = NS_random(ns, total_squares) / 50;
		#else
		count = NS_random(ns, total_squares) / 20;
		#endif
		
		for (i=0; i<count; i++)
		{
			for (try=0; try<25; try++)
			{
				//modified by Joey Adams:  moved cluster_loc.x and cluster_loc.y outside of the random function for true clusters to happen
				x = NS_random(ns, cluster_size.x) + cluster_loc.x;
				y = NS_random(ns, cluster_size.y) + cluster_loc.y;
				map_index = (long)y * (long)ns->width + (long)x;
				if (board[map_index] == Floor)
				{
					board[map_index] = Mine;
					break;
				}
			}
			
			//make first mine the pulse generator if desired
			if (i==0 && count>1 && ns->settings.mine_detonators)
				board[map_index] = MineDetonator;
		}
	}
	
	//add ammo packs
	if (ns->settings.bullet_ammo)
	{
		#if SMALL_DEVICE==0
		//randomly place a random number of ammo packs (1% max)
		count = NS_random(ns, total_squares) / 100;
		#else
		count = NS_random(ns, total_squares) / 40;
		#endif
		
		for (i=0; i<count; i++)
		{
			//modified by Joey Adams:  see the mines section
			x = NS_random(ns, cluster_size.x) + cluster_loc.x;
			y = NS_random(ns, cluster_size.y) + cluster_loc.y;
			map_index = (long)y * (long)ns->width + (long)x;
			board[map_index] = AmmoPack;
		}
	}
	
	//add rocket packs
	//modified by Joey Adams:  ammo works differently in this Nuke Snake implementation
	if (ns->settings.rocket_ammo)
	{
		#if SMALL_DEVICE==0
		//randomly place a random number of rockets (0.5% max)
		count = NS_random(ns, total_squares) / 200;
		#else
		count = NS_random(ns, total_squares) / 80;
		#endif
		
		for (i=0; i<count; i++)
		{
			//modified by Joey Adams:  see the mines section
			x = NS_random(ns, cluster_size.x) + cluster_loc.x;
			y = NS_random(ns, cluster_size.y) + cluster_loc.y;
			map_index = (long)y * (long)ns->width + (long)x;
			board[map_index] = RocketPack;
		}
	}
	
	//add nukes
	if (ns->settings.nuke_ammo)
	{
		#if SMALL_DEVICE==0
		//randomly place a random number of nukes (0.2% max)
		count = NS_random(ns, total_squares) / 400;
		#else
		count = NS_random(ns, total_squares) / 160;
		#endif
		
		for (i=0; i<count; i++) {
			//modified by Joey Adams:  see the mines section
			x = NS_random(ns, cluster_size.x) + cluster_loc.x;
			y = NS_random(ns, cluster_size.y) + cluster_loc.y;
			map_index = (long)y * (long)ns->width + (long)x;
			board[map_index] = NukePack;
		}
	}
	
	//add border
   if (ns->settings.walls) {
      map_index = 0;
      for (y=1; y<=ns->height; y++)
      {
         board[map_index] = Wall;
         map_index += ns->width-1;
         board[map_index] = Wall;
         map_index++;
      }
      map_index = (ns->height-1) * ns->width;
      for (x=1; x<=ns->width-2; x++)
      {
         board[x] = Wall;
         board[map_index + x] = Wall;
      }
   }
}

//Set up the board
//this function isn't burdened with positioning players
//the board is completely cleared (i.e. only floor) when layout() is called
void layout(NS *ns)
{
	InitMap(ns);
	return;
	signed char *ptr;
	//Create border
	if (ns->settings.walls)
	{
		short w;
		short h;
		w=ns->width-1;
		ptr=ns->board;
		do *ptr++=Wall; while (w--);
		w=ns->width-1;
		h=ns->height-2;
		ptr+=ns->width*h;
		do *ptr++=Wall; while (w--);
		w=ns->width-1;
		h=ns->height-2;
		ptr=ns->board;
		do
		{
			*ptr=Wall;
			ptr+=w;
			*ptr++=Wall;
		} while (h--);
	}
	if (ns->width==50 && ns->height==36)
		setup_testing_board(ns, maze_testing_board);
	if (ns->width==20 && ns->height==12)
		setup_testing_board(ns, micro_testing_board_less);
}
