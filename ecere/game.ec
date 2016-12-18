#include <stdio.h>

#define Error(str,...) fprintf(stderr,"ERROR:  " str "\n", ##__VA_ARGS__ )
#define Glitch(str,...) fprintf(stderr,"GLITCH:  " str "\n", ##__VA_ARGS__ )
#define Bug(str,...) fprintf(stderr,"BUG:  " str "\n", ##__VA_ARGS__ )
#define Fatal(str,...) fprintf(stderr,"FATAL:  " str "\n", ##__VA_ARGS__ )

#define Cell(x,y) (board[(y)*width+(x)])
#define CellOverlay(x,y) (overlay[(y)*width+(x)])

import "layout"

public:

import "ai"

define NS_MAX_FRAMEDROP = 10;

class NukeSnake {
public:
   void *master;
   
   Tile *board; //board==NULL means there is no game open right now
      //The board's data consists of two parts, each width*height Tiles
         //The current half is the cells the game engine will write to in order to request draws of cells
         //The actual half is what is drawn to the screen at the last redraw
      //Both board halves are used in NukeSnake::Draw in order to bring the actual half up to date
   uint width, height;
   Tile *overlay; //This is where explosions and respawn animations are drawn
   
   uint phase; //counts down
   bool paused;
   
   NS_Settings settings;
   
   //these should not be changed except after a NewGame()
   uint player_max, bullet_max, effect_max;
   player_max=2;
   bullet_max=256;
   effect_max=256;
   
   NS_Player *players; //persists while the player is in the game
   
   NS_Snake *snakes; //persists for each life of the player
   NS_Bullet *bullets;
   NS_Effect *effects;
   
   NukeSnake() {
      settings.Default();
   }
   ~NukeSnake() {
      NewGame();
   }
   void NewGame(void) {
      phase = 59;
      paused = false;
      ClearBoard();
      ClearPlayers();
      ClearObjects();
   }
   NS_Player *NewPlayer(NS_Controller controller, Tile icon) {
      NS_Player *p;
      uint d;
      if (!players)
         InitPlayers();
      p = players;
      d = player_max;
      for (;d--;p++) {
         if (!p->controller) {
            memset(p, 0, sizeof(NS_Player));
            p->controller = controller;
            p->icon = icon;
            p->bullet_count = 5;
            controller.ns = this;
            controller.player = p;
            controller.Init();
            return p;
         }
      }
      delete controller;
      return null;
   }
   void NewRound(uint nwidth, uint nheight) {
      ClearBoard();
      InitBoard(nwidth, nheight);
      ClearObjects();
      InitObjects();
      if (players)
      {  NS_Player *p=players; uint d=player_max;
         for (;d--;p++) if (p->controller)
            RespawnPlayer(p);
      }
   }
   void Frame(void) {
      
   }
   //draws the tiles that were changed from last Draw
   //returns true if something was actually redrawn
   bool Draw(void)
   {
      uint x,y;
      const Tile *board = this.board;
      const Tile *overlay = this.overlay;
      Tile *actual = board+width*height;
      bool ret = false;
      
      for (y=0; y<height; y++)
      for (x=0; x<width;  x++, board++, overlay++, actual++)
      {
         Tile thistile = !*overlay ? *board : *overlay;
            //thistile = the board tile unless the corresponding overlay tile is used
         if (*actual != thistile)
         {
            *actual = thistile;
            if (thistile<=0 || thistile>=(Tile)Tile::enumSize)
               thistile = 0;
            OnDrawCell(master, x, y, thistile);
            ret = true;
         }
      }
      
      return ret;
   }
   void UpdateAll(void) {
      memset(board+width*height, 0, width*height*sizeof(Tile));
      Draw();
   }
   
   virtual void Instance::OnDrawCell(uint x, uint y, Tile tile);
   
private:
   
   void ClearBoard(void) {
      delete board;
      delete overlay;
   }
   void ClearPlayers(void) {
      if (players) {
         NS_Player *p = players;
         uint count = player_max;
         for (;count--;p++)
            delete p->controller;
         delete players;
      }
   }
   void ClearObjects(void) {
      if (snakes) {
         NS_Snake *p = snakes;
         uint count = player_max;
         for (;count--;p++) {
            if (p->player)
               p->player->snake = null;
         }
      }
      delete snakes;
      delete bullets;
      delete effects;
   }
   void InitPlayers(void) {
      players = new0 NS_Player[player_max];
   }
   void InitObjects(void) {
      snakes = new0 NS_Snake[player_max];
      bullets = new0 NS_Bullet[bullet_max];
      effects = new0 NS_Effect[effect_max];
   }
   void InitBoard(uint nwidth, uint nheight) {
      if (!nwidth)
         nwidth=1;
      if (!nheight)
         nheight=1;
      width = nwidth;
      height = nheight;
      board = new0 Tile[width*height*2];
      overlay = new0 Tile[width*height];
      {  uint e; Tile *tp;
         for (tp=board,e=width*height; e--;)
            *tp++ = floor;
      }
      
      NS_Layout(this);
   }
   
   void RespawnPlayer(NS_Player *player) {
      NS_Effect *e;
      uint x,y;
      bool kill = false;
      if (player->snake) {
         x = player->snake->x;
         y = player->snake->y;
         kill = true;
      } else {
         uint position = FindEmptySpace();
         x = position % width;
         y = position / width;
      }
      
      e = NewEffect(respawn, x,y);
      if (!e)
         return;
      e->respawn = player;
      
      if (!kill) {
         e->frame -= 16;
         CellOverlay(x,y) = e->GetIcon();
      }
   }
   NS_Bullet *NewBullet(BulletType type, uint x, uint y, Direction direction) {
      
   }
   NS_Effect *NewEffect(EffectType type, uint x, uint y) {
      
      
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
};

struct NS_Settings {
   uint
      mines:1, mine_detonators:1, clusters:1, 
      bullets:1, bullet_ammo:1, rockets:1, rocket_ammo:1, nukes:1, nuke_ammo:1,
      linear_terrain:1, fractal_terrain:1, stop_at_water:1,
      diagonals:1, walls:1, rubber_walls:1,
      tails:1, explosion_blocks:1, total_war:1,
      small_device:1;
      //total_war means there are no new rounds; players simply respawn
      //small_device provides tweaks geared toward making NukeSnake with small map sizes nicer
   uint bullet_speed; //speed in relation to player (should be 2...6)
   
   void Default(void) {
      mines            = 1;
      mine_detonators  = 1;
      clusters         = 1;
      bullets          = 1;
      bullet_ammo      = 0;
      rockets          = 1;
      rocket_ammo      = 1;
      nukes            = 1;
      nuke_ammo        = 1;
      linear_terrain   = 1;
      fractal_terrain  = 1;
      stop_at_water    = 1;
      diagonals        = 1;
      walls            = 1;
      rubber_walls     = 0;
      tails            = 0;
      explosion_blocks = 0;
      total_war        = 0;
      small_device     = 0;
      
      bullet_speed     = 6;
   }
};

class NS_Controller {
public:
   const NukeSnake ns;
   const NS_Player *player;
   virtual void Init(void);
};
struct NS_Player {
   NS_Controller controller;
   NS_Snake *snake; //null if he's dead
   Tile icon; //default icon for the snake
   uint bullet_count; //only matters if settings.bullet_ammo==1
   uint rocket_count; //only matters if settings.rocket_ammo==1
   uint nuke_count; //only matters if settings.nuke_ammo==1
   uint deaths;
};
struct NS_Snake {
   NS_Player *player;
   Direction dir;
   uint x,y;
   Tile covering; //The tile this object replaced to be drawn at (x,y).  It will restore this tile when it moves
   int phase;
   
   bool has_tail;
   int fire_phase; //counts down, <=0 means the player can fire right now (minimum is -bullet_maxphase, after-fire is +bullet_maxphase)
   int trudge; //number of updates to wait before player can move
};
struct NS_Bullet {
   BulletType type;
   Direction dir;
   uint x,y;
   Tile covering; //see description in NS_Player::covering
   int phase; //counts down
};
struct NS_Effect {
   EffectType type; //Any of EffectTypes or 0 for hole
   int frame; //counts down
   uint x,y;
   int phase; //counts down
   
   Player *respawn;
      //players[respawn] will respawn at the end of this effect if type==respawn
      //a respawn effect will be generated at the end of this effect if type == ET_Explosion and respawn>=0
   
   Tile GetIcon(void) {
      switch (type)
      {
         case explosion:
            return explode1+frame;
            break;
         case respawn:
            if (frame>=12)
               return (frame&2) ? explode2 : Floor;
            else
               return (frame&2) ? respawn->icon : Floor;
            break;
      }
      return floor;
   }
};

enum Tile {
   null=0, floor,
   bluePiece, redPiece,
   bulletH, bulletV, bulletF, bulletB,
   rocketH, rocketV, rocketF, rocketB,
   nukeH, nukeV, nukeF, nukeB,
   explode1, explode2,
   ammoPack, rocketPack, nukePack,
   mine, mineDetonator, coal, tree, wall, water, swamp,
   blueTail, redTail
};
const TileClass nukesnake_tile_class[Tile] = {
   floor, floor,
   player, player,
   bullet, bullet, bullet, bullet,
   bullet, bullet, bullet, bullet,
   bullet, bullet, bullet, bullet,
   explosion, explosion, //no normal tiles should be set to the explosion icons, but the overlay can (and often is)
   other, other, other,
   other, other, other, other, other, other, other,
   tail, tail
};
enum TileClass {
   floor=0,
   player,
   bullet, //includes rockets
   explosion,
   tail,
   other
};
enum BulletType {
   bullet=1, rocket=2, nuke=3
};
enum EffectType {
   explosion=1, respawn=2
};
enum EventType {
   explosion, playerKilled,
   bulletFired, rocketFired, nukeFired,
   ammoPickedUp, rocketPickedUp, nukePickedUp, playerLost
};
enum MeansOfDeath {
   player, bullet, rocket, nuke, explosion, nukeRadius, mine, coal, wall, tail, shotHimself
};
enum Direction {
   noDirection=0, right, downRight, down, downLeft, left, upLeft, up, upRight
};
