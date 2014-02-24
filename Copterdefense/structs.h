#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED



#include "sdl2d.h"



enum enemyType
{
    AlreadySpawned = 0,
    AlienAirMine = 1,
    AlienDownTentacle = 2,
    AlienGiantTentacle = 3,
    AlienUpTentacle = 4,
    AlienAirTransporter = 5,
    AlienGroundTransporter = 6,
    AlienScout = 7,
    AlienRocket = 8,
    AlienSeekingRocket = 9,
    AlienSapper = 10,
    AlienPlanter = 11,
    AlienFighter = 12,
    AlienGunship = 13,
    AlienTank = 14,
    AlienAirStation = 15,
    AlienGroundStation = 16,
    AlienBomber = 17,
    AlienBulletLeft = 18,
    AlienBulletRight = 19,
    AlienBulletUp = 20,
    AlienBulletDown = 21,
    HumanSupply = 22,
    HumanBox = 23
};


struct enemySpawn
{
    unsigned int spawnTime;
    int x;
    int y;
    enemyType type;
};


struct enemySpawnList
{
    enemySpawnList *before;
    enemySpawn *element;
    enemySpawnList *next;
};


struct enemy
{
    Object2D *object;
    enemyType type;
    int collisionDamage;
    int lifes;
};


struct enemyList
{
    enemyList *before;
    enemy *element;
    enemyList *next;
};


struct level
{
    Window *gameWindow;
    Mix_Music *backgroundMusic;
    enemySpawnList *enemys;
};


struct objectList
{
    objectList *before;
    Object2D *element;
    objectList *next;
};


struct alienBullet
{
    Object2D *object;
    int movementX;
    int movementY;
};



#endif
