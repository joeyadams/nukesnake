
#define uint uint_
#include <stdlib.h>
#undef uint

public:

import "game"

//Laying out the board is given its own source file because of the complexity associated with linear and cluster (a.k.a. fractal) terrain

uint rand32(void)
{
        //randintany assumes RAND_MAX is 32767 or any greater value of 2^n-1 where n is an integer (like 65535, 131071...).
        uint ret=rand();
        ret|=((uint)rand())<<15;
        ret|=((uint)rand())<<30;
        return ret;
}

uint rand_ulong(uint n)
{
        if (n<=1)
                return 0;
        return rand32()%n;
                //this does theoretically have slight preference for lower numbers
}

private struct coord {
   uint x,y;
};

//This function was translated from Pascal out of David Riggle's Win Snake 2.0
//TODO:  Check division and modulus
void NS_Layout(NukeSnake ns)
{
   int n, thetry;
   uint i,count;
   uint total_squares;
   uint x, y;
   uint map_index;
   Tile there;
   int average, delta;
   coord cluster_loc, cluster_size;
   Tile *board = ns.board;
   
   total_squares = ns.width * ns.height;
   
   for (i=0; i<total_squares; i++)
      board[i] = floor;
   
   //add fractal terrain
   if (ns.settings.fractal_terrain) {
      //rock (coal), swamp, trees, river, blank
      //build fractal terrain map
      
      //iterate a few times with decreasing magnitudes
      if (!ns.settings.small_device)
         delta=24;
      else
         delta=16;
      while (delta>0) {
         //perturb each square
         for (i=ns.width+1; i<total_squares-ns.width-1;i++) {
            //average surrounding squares
            average = board[i-ns.width-1]+board[i-ns.width]+board[i-ns.width+1]+
                      board[i-1]+board[i]+board[i+1]+
                      board[i+ns.width-1]+board[i+ns.width]+board[i+ns.width+1];
            average/=9;
            //add random delta
            if (rand32()&1)
               average+=delta;
            else
               average-=delta;
            board[i] = (Tile)average;
         }
         //decrease delta
         delta>>=1;
      }
      
      //set terrain based on altitude
      for (i=0;i<total_squares;i++) {
         int tile = (int)board[i];
         if (ns.settings.small_device)
            tile*=2; //diversify terrain
         if (tile<-18 || tile>=11)
            there = coal;
         else if (tile<=-12)
            there = water;
         else if (tile<=-8)
            there = swamp;
         else if (tile<=4)
            there = floor;
         else if (tile<=10)
            there = tree;
         else
            there = coal; //flow should never reach here
         board[i] = there;
      }
   }
   
   //add linear terrain
   if (ns.settings.linear_terrain) {
      //Random walk terrain
      //pick a random spot & grow a river
      for (n=0;n<2;n++) { //once for trees, once for rivers
         there = n?tree:water;
         x = (rand_ulong(ns.width)+ns.width) / 3;
         y = (rand_ulong(ns.height)+ns.height) / 3;
         count = rand_ulong(total_squares);
         for (i=0;i<count;i++) {
            map_index = y*ns.width + x;
            board[map_index] = there;
            board[map_index+1] = there;
            switch (rand_ulong(4)) {
               case 0:
                  break;
               case 1:
               case 2:
                  if (y<ns.height-2)
                     y++;
                  break;
               case 3:
                  if (y>1)
                     y--;
                  break;
            }
            switch (rand_ulong(4)) {
               case 0:
                  break;
               case 1:
               case 2:
                  if (x<ns.width-2)
                     x++;
                  break;
               case 3:
                  if (x>1)
                     x--;
                  break;
            }
            //grow a swamp, forest, or rock garden
            if (!rand_ulong(25)) {
               Tile selections[4] = {coal, swamp, tree, water};
               there = selections[rand_ulong(4)];
            }
         }
      }
   }
   
   //set cluster size & location
   //TODO:  Audit cluster size/location for potential buffer overruns
   if (ns.settings.clusters) {
      uint min = ns.width;
      if (ns.height<min)
         min = ns.height;
      cluster_size.y = rand_ulong(min);
      if (cluster_size.y == 0)
         cluster_size.y = 1;
      cluster_size.x = cluster_size.y;
      cluster_loc.y = rand_ulong(ns.height - cluster_size.y);
      cluster_loc.x = rand_ulong(ns.width - cluster_size.x);
   }
   else {
      cluster_loc.x = 0;
      cluster_loc.y = 0;
      cluster_size.x = ns.width;
      cluster_size.y = ns.height;
   }
   
   //add mines
   if (ns.settings.mines) {
      if (!ns.settings.small_device)
         //randomly place a random number of mines on blank ground (2% max)
         count = rand_ulong(total_squares) / 50;
      else
         count = rand_ulong(total_squares) / 20;
      
      for (i=0; i<count; i++) {
         for (thetry=0; thetry<25; thetry++) {
            //modified by Joey Adams:  moved cluster_loc.x and cluster_loc.y outside of the random function for true clusters to happen
            x = rand_ulong(cluster_size.x) + cluster_loc.x;
            y = rand_ulong(cluster_size.y) + cluster_loc.y;
            map_index = y * ns.width + x;
            if (board[map_index] == floor) {
               board[map_index] = mine;
               break;
            }
         }
         
         //make first mine the pulse generator if desired
         if (i==0 && count>1 && ns.settings.mine_detonators)
            board[map_index] = mineDetonator;
      }
   }
   
   //add ammo packs
   if (ns.settings.bullet_ammo) {
      if (!ns.settings.small_device)
         //randomly place a random number of ammo packs (1% max)
         count = rand_ulong(total_squares) / 100;
      else
         count = rand_ulong(total_squares) / 40;
      
      for (i=0; i<count; i++) {
         //modified by Joey Adams:  see the mines section
         x = rand_ulong(cluster_size.x) + cluster_loc.x;
         y = rand_ulong(cluster_size.y) + cluster_loc.y;
         map_index = y * ns.width + x;
         board[map_index] = ammoPack;
      }
   }
   
   //add rocket packs
   //modified by Joey Adams:  ammo works differently in this Nuke Snake implementation
   if (ns.settings.rocket_ammo) {
      if (!ns.settings.small_device)
         //randomly place a random number of rockets (0.5% max)
         count = rand_ulong(total_squares) / 200;
      else
         count = rand_ulong(total_squares) / 80;
      
      for (i=0; i<count; i++) {
         //modified by Joey Adams:  see the mines section
         x = rand_ulong(cluster_size.x) + cluster_loc.x;
         y = rand_ulong(cluster_size.y) + cluster_loc.y;
         map_index = y * ns.width + x;
         board[map_index] = rocketPack;
      }
   }
   
   //add nukes
   if (ns.settings.nuke_ammo) {
      if (!ns.settings.small_device)
         //randomly place a random number of nukes (0.2% max)
         count = rand_ulong(total_squares) / 400;
      else
         count = rand_ulong(total_squares) / 160;
      
      for (i=0; i<count; i++) {
         //modified by Joey Adams:  see the mines section
         x = rand_ulong(cluster_size.x) + cluster_loc.x;
         y = rand_ulong(cluster_size.y) + cluster_loc.y;
         map_index = y * ns.width + x;
         board[map_index] = nukePack;
      }
   }
   
   //add border
   if (ns.settings.walls) {
      map_index = 0;
      for (y=1; y<=ns.height; y++) {
         board[map_index] = wall;
         map_index += ns.width-1;
         board[map_index] = wall;
         map_index++;
      }
      map_index = (ns.height-1) * ns.width;
      for (x=1; x<=ns.width-2; x++) {
         board[x] = wall;
         board[map_index + x] = wall;
      }
   }
}
