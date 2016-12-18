import "ecere"
import "game"

class Form1 : Window
{
   icon = { ":icon.bmp" };
   text = "Form1";
   background = activeBorder;
   borderStyle = sizable;
   hasMaximize = true;
   hasMinimize = true;
   hasClose = true;
   size = { 576, 432 };
   
   Surface surface;
   
   NukeSnake nukesnake {this;
      void OnDrawCell(uint x, uint y, Tile tile) {
         surface.Blit(tile_sprites, x<<4,y<<4, 0,(uint)tile<<4, 16, 16);
      }
   };
   
   Bitmap tile_sprites {};
   
   bool OnCreate(void) {
      NS_Controller a {};
      NS_Controller b {};
      nukesnake.NewGame();
      nukesnake.NewPlayer(a, bluePiece);
      nukesnake.NewPlayer(b, redPiece);
      nukesnake.NewRound(clientSize.w>>4, clientSize.h>>4);
   }

   bool OnLoadGraphics()
   {
      tile_sprites.Load(":graphics.bmp", "bmp", null);
      tile_sprites.MakeDD(displaySystem);
      return true;
   }

   void OnRedraw(Surface surface)
   {
      this.surface = surface;
      nukesnake.UpdateAll();
      //surface.Blit(tile_sprites, 10,10, 0,0, tile_sprites.width, tile_sprites.height);
   }
}

Form1 form1 {};
