#ifndef MAIN_H
#define MAIN_H

enum SoundTypes
{
        S_Ammo, S_Die, S_Explosion, S_Fire, S_Gameover, S_Gamepoint, S_Get
rocket, S_Rocket,
        S_TypeCount
};

void play_sound(short s);

#endif
