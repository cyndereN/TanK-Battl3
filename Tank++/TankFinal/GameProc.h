#ifndef _GAME_PROC_H_
#define _GAME_PROC_H_

#include "Entity.h"

enum LEVEL {OPEN, LEVEL1, LEVEL2_OPEN, LEVEL2, SUCCEED, FAIL};

void EnterKey(int key);

void NewEntity();

void ChangeWndSize(int w, int h);

void ChangeLevel(LEVEL newL);

void ResetPlayer(Entity *player);

void Draw(HDC hdc, HWND hWnd);

void DrawScene(HDC hdc, LEVEL curL);

void DrawGameScene(HDC hdc);

void Update(int ts);

void EnemyBirth();

void Destroy();

void NextFrame(int ts);

#endif
