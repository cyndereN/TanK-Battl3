#ifndef _INIT_H_ 
#define _INIT_H_

#include "GameProc.h"

int GetAnimStep();

int GetTimeStep();

void ReadIni(LEVEL l);

void InitMap(LEVEL l, Entity *headBlock);

void Init(LEVEL l);

void InitEntity(Entity *headEnemy, Entity *headBlock, Entity *headBullet, Entity *headEnemyBullet, Entity *player, LEVEL l);

HBITMAP GetBackPic();

int RandomFire();

int RandomDir();

int GetBirthTime();

void EnemyBirth(Entity* headEnemy, int type, int w);

int GetPassScore();

HBITMAP GetLogoPic();

HBITMAP GetSuccPic();

HBITMAP GetFailPic();
#endif