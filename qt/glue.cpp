#include "glue.h"
#include "../engine/game.h"

extern "C" unsigned long rand32(void)
{
        //randintany assumes RAND_MAX is 32767 or any greater value of 2^n-1 where n is an integer (like 65535, 131071...).
        unsigned long ret=rand();
        ret|=((unsigned long)rand())<<15;
        ret|=((unsigned long)rand())<<30;
        return ret;
}

extern "C" unsigned long rand_ulong(unsigned long n)
{
        if (n<=1)
                return 0;
        return rand32()%n;
                //this does theoretically have slight preference for lower numbers
}

extern "C" void DrawCell(short x,short y,short icon)
{
	
}

extern "C" void ClearScreen(void)
{
	
}

//returns most recent directional indication for a given player
//PT_Right=1, PT_Left=2 (, PT_Network=3)
extern "C" short GetPlayerDir(short type)
{
	
}

//returns nonzero if the player pressed the fire key, zero otherwise
//PT_Right=1, PT_Left=2 (, PT_Network=3)
extern "C" short GetPlayerFire(short type)
{
	
}

//handles a directional keypress for a player (so that the right player's 4 and 8 will accumulate to be 7, for instance)
//used by main.c
//if diagonal_compat is set, non-diagonal keys can be combined for a diagonal result
extern "C" void AcceptDirKey(short type, short direction, short diagonal_compat)
{
	
}

//handles a fire keypress for a player
extern "C" void AcceptFireKey(short type,short val)
{
	
}

//used in game.c for initialization of game or round
extern "C" void ClearAllKeys(void)
{
	
}

//clears the player direction key data in preparation for the next frame
//used by game.c
extern "C" void ClearPlayerDirKeys(void)
{
	
}

//Reports an event to the glue.  In the future, this may include carnage reporting, for instance
//Currently, it is just for spawning sounds
extern "C" void GlueEvent(short type, short param, short player, unsigned short x, unsigned short y)
{
	
}

extern "C" void UpdateScores(void)
{
	
}
