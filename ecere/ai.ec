public:

import "game"

struct NS_AI
{
   //logical
   uint last_x,last_y;
   Direction last_dir;
   
   //emotional
   int anger;
   int angry_towards;
};
