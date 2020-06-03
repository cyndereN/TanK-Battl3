#include "framework.h"
#include "GameProc.h"
#include "Init.h"

int nScore = 0;				
int bFire = 0;				
int wndWidth = 0;	int wndHeight = 0;  
LEVEL curLevel = OPEN;		

Entity *pHeadBlocks = NULL;	
Entity *pHeadEnemys = NULL;	
Entity *pPlayer = NULL;		
Entity *pHeadBullets = NULL;
Entity *pHeadEnemyBullets = NULL;

extern int reset_time;		


void EnemyBirth()
{
	if (curLevel != LEVEL1 && curLevel != LEVEL2)	
		return;

	int type = 0;
	switch (curLevel)
	{
	case LEVEL1:
		type =	(rand()%3)== 0 ? 1: 0;	
		break;
	case LEVEL2:
		type =	(rand()%3)== 0 ? 0: 1;	
		break;
	}

	EnemyBirth(pHeadEnemys, type, wndWidth);
}

void EnterKey(int key)
{
	switch(curLevel)
	{
	case OPEN:					
		if (key == VK_RETURN)
			ChangeLevel(LEVEL1);
		break;
	case LEVEL2_OPEN:			
		if (key == VK_RETURN)
			ChangeLevel(LEVEL2);
		break;
	case FAIL:
	case SUCCEED:
		if (key == VK_RETURN)
			ChangeLevel(OPEN);
		break;
	case LEVEL1:
	case LEVEL2:
		switch (key)	
		{
		case VK_LEFT:
			pPlayer->dir = LEFT;
			pPlayer->p = 0;
			break;
		case VK_RIGHT:
			pPlayer->dir = RIGHT;
			pPlayer->p = 0;
			break;
		case VK_UP:
			pPlayer->dir = UP;
			pPlayer->p = 0;
			break;
		case VK_DOWN:
			pPlayer->dir = DOWN;
			pPlayer->p = 0;
			break;
		case VK_SPACE:
			bFire = 1;
			break;
		}		
		break;
	}
}

void NewEntity()
{
	if(pHeadBlocks == NULL)
	{
		pHeadBlocks = (Entity*)malloc(sizeof(Entity));
		if (pHeadBlocks == NULL)
			return;
		pHeadBlocks->next = NULL;
	}
	if(pHeadEnemys == NULL)
	{
		pHeadEnemys = (Entity*)malloc(sizeof(Entity));
		if (pHeadEnemys == NULL)
			return;
		pHeadEnemys->next = NULL;
	}
	if(pPlayer == NULL)
	{
		pPlayer = (Entity*)malloc(sizeof(Entity));
		if (pPlayer == NULL)
			return;
		pPlayer->next = NULL;
	}
	if(pHeadBullets == NULL)
	{
		pHeadBullets = (Entity*)malloc(sizeof(Entity));
		if (pHeadBullets == NULL)
			return;
		pHeadBullets->next = NULL;
	}
	if(pHeadEnemyBullets == NULL)
	{
		pHeadEnemyBullets = (Entity*)malloc(sizeof(Entity));
		if (pHeadEnemyBullets == NULL)
			return;
		pHeadEnemyBullets->next =NULL;
	}
}

void ChangeWndSize(int w, int h)
{
	wndWidth = w, wndHeight = h;
}

void ChangeLevel(LEVEL newL)
{
	curLevel = newL;
	Init(newL);		
	if (newL != LEVEL1 && newL != LEVEL2)	
		return;
	NewEntity();	
	InitEntity(pHeadEnemys, pHeadBlocks, pHeadBullets, pHeadEnemyBullets, pPlayer, newL);	
	ResetPlayer(pPlayer);	
	nScore = 0;		
	reset_time = 1;
}

void ResetPlayer(Entity *player)
{
	player->p = 1;
	player->dir = UP;
	player->x = wndWidth/2;
	player->y = wndHeight-player->s;
}

void Draw(HDC hdc, HWND hWnd)
{
	HDC memHDC = CreateCompatibleDC(hdc);

	RECT rectClient;
	GetClientRect(hWnd, &rectClient);

	HBITMAP bmpBuff = CreateCompatibleBitmap(hdc,wndWidth,wndHeight);
	HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
	PatBlt(memHDC,0,0,wndWidth,wndHeight,WHITENESS);	

	DrawScene(memHDC, curLevel);

	BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, wndWidth,
		wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

	SelectObject(memHDC, pOldBMP);
	DeleteObject(bmpBuff);
	DeleteDC(memHDC);
}

void DrawScene(HDC hdc, LEVEL curL)
{
	HFONT hf;
	WCHAR str[32];
	str[31] = '\0';
	SetBkMode(hdc, TRANSPARENT);	
	long lfHeight;
	lfHeight = -MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	HFONT hfOld;
	hf = CreateFont(lfHeight, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	hfOld = (HFONT)SelectObject(hdc, hf);
	SetTextColor(hdc, RGB(122, 0, 0));
	HBITMAP cb = GetLogoPic();
	switch (curL)
	{
	case OPEN:		
		cb = GetLogoPic();
		wsprintf(str, L"%s", L"按下回车键开始游戏");	
		break;
	case LEVEL1:	
	case LEVEL2:
		SetTextColor(hdc, RGB(100, 0, 0));
		lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		DeleteObject(hf);
		hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
		SelectObject(hdc, hf);
		wsprintf(str, L"Life:%d Score:%d", pPlayer->life, nScore);	
		DrawGameScene(hdc);
		break;
	case LEVEL2_OPEN:
		cb = GetSuccPic();
		wsprintf(str, L"%s", L"按下回车键开始下一关");	
		break;
	case SUCCEED:	
		cb = GetSuccPic();
		wsprintf(str, L"%s", L"胜利！按下回车键重新开始");	
		break;
	case FAIL:	
		cb = GetFailPic();
		wsprintf(str, L"%s", L"失败！按下回车键重新开始");	
		break;
	}
	if (curL==OPEN||curL==LEVEL2_OPEN||curL==FAIL||curL==SUCCEED)
	{
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, cb);
		BITMAP bm;
		GetObject(cb, sizeof(bm), &bm);
		SetStretchBltMode(hdc,STRETCH_HALFTONE);
		BitBlt(hdc, wndWidth/2-bm.bmWidth/2, wndHeight/2-bm.bmHeight/2, wndWidth, wndHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

	}
	TextOut(hdc, wndWidth/2-100, wndHeight-100,str,wcslen(str));

	SelectObject(hdc, hfOld);		
	DeleteObject(hf);
}

void DrawGameScene(HDC hdc)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, GetBackPic());
	BITMAP bm;
	GetObject(GetBackPic(), sizeof(bm), &bm);
	SetStretchBltMode(hdc,STRETCH_HALFTONE);
	BitBlt(hdc, 0, 0, wndWidth, wndHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);


	DrawEntities(hdc, pHeadEnemys);
	DrawEntities(hdc, pHeadBullets);
	DrawEntities(hdc, pHeadEnemyBullets);
	DrawEntities(hdc, pHeadBlocks);
	DrawEntity(hdc, pPlayer);
}

void Destroy()
{
	Destroy(pHeadBlocks);
	Destroy(pHeadEnemys);
	Destroy(pHeadBullets);
	Destroy(pHeadEnemyBullets);

	free(pPlayer);
	free(pHeadBlocks);
	free(pHeadEnemys);
	free(pHeadBullets);
	free(pHeadEnemyBullets);

	pHeadBlocks = NULL;		
	pHeadEnemys = NULL;		
	pPlayer = NULL;			
	pHeadBullets = NULL;	
	pHeadEnemyBullets = NULL;
}

void NextFrame(int ts)
{
	NextFrameEntites(pHeadEnemys, ts);
	NextFrameEntites(pHeadBullets, ts);
	NextFrameEntites(pHeadEnemyBullets, ts);
	NextFrame(pPlayer, ts);
}

void Update(int ts)
{
	if (curLevel != LEVEL1 && curLevel != LEVEL2)
		return;

	MoveEntities(pHeadEnemys, ts);
	MoveEntities(pHeadBullets, ts);
	MoveEntities(pHeadEnemyBullets, ts);
	Move(pPlayer, ts);

	if (bFire)
	{
		Fire(pPlayer, pHeadBullets);
		bFire = 0;
	}

	Entity *freeP = NULL;

	for(Entity *ip = pHeadBullets->next; ip != NULL; ip = ip->next)	
	{
		if (freeP != NULL)				
		{
			Destroy(pHeadBullets, freeP);
			freeP = NULL;
		}
		for (Entity *jp = pHeadEnemys->next; jp != NULL; jp = jp->next)
		{
			if (IsCollider(ip, jp))		
			{
				freeP = ip;				
				jp->life--;
				if (jp->life <= 0)
				{
					Destroy(pHeadEnemys, jp);	
					nScore++;				
				}
				break;
			}
		}
	}
	if (freeP != NULL)				
	{
		Destroy(pHeadBullets, freeP);
		freeP = NULL;
	}

	
	for (Entity *p = pHeadEnemyBullets->next; p != NULL; p = p->next)
	{
		if (IsCollider(p, pPlayer))
		{
			Destroy(pHeadEnemyBullets, p);
			ResetPlayer(pPlayer);
			pPlayer->life--;
			break;
		}
	}
	
	for (Entity *p = pHeadEnemys->next; p != NULL; p = p->next)
	{
		if (RandomFire())
			Fire(p, pHeadEnemyBullets);


		if (IsCollider(pPlayer,p))
		{
			ResetPlayer(pPlayer);
			pPlayer->life--;
		}
	}

	for (Entity *p = pHeadEnemys->next; p != NULL; p = p->next)
	{
		if (!WallCollider(p, wndWidth, wndHeight))		
		{
			int cg = 0;
			for (Entity *q = pHeadBlocks->next; q != NULL; q = q->next)	
			{
				if (IsCollider(p, q))
				{
					cg = 1;

					Move(p, -ts);					
					break;
				}
			}
			if (RandomDir() || cg)
				p->dir = Dir((p->dir+1+rand()%3)%4);
		}
		else
			p->dir = Dir((p->dir+1+rand()%3)%4);

	}

	for (Entity *p = pHeadBlocks->next; p != NULL; p = p->next)	
	{
		if (IsCollider(p, pPlayer))
		{
			switch(pPlayer->dir)
			{
			case UP:
				pPlayer->y = p->y+(p->s+pPlayer->s)/2;
				break;
			case DOWN:
				pPlayer->y = p->y-(p->s+pPlayer->s)/2;
				break;
			case LEFT:
				pPlayer->x = p->x+(p->s+pPlayer->s)/2;
				break;
			case RIGHT:
				pPlayer->x = p->x-(p->s+pPlayer->s)/2;
				break;
			}
			pPlayer->p = 1;
			break;
		}
	}

	for (Entity *p = pHeadBullets->next; p != NULL; p = p->next)	
	{
		if (freeP != NULL)
		{
			Destroy(pHeadBullets, freeP);
			freeP = NULL;
		}
		if (WallCollider(p, wndWidth, wndHeight))
			freeP = p;
	}
	if (freeP != NULL)
	{
		Destroy(pHeadBullets, freeP);
		freeP = NULL;
	}

	for (Entity *p = pHeadEnemyBullets->next; p != NULL; p = p->next)	
	{
		if (freeP != NULL)
		{
			Destroy(pHeadEnemyBullets, freeP);
			freeP = NULL;
		}
		if (WallCollider(p, wndWidth, wndHeight))
			freeP = p;
	}
	if (freeP != NULL)
	{
		Destroy(pHeadEnemyBullets, freeP);
		freeP = NULL;
	}


	if (WallCollider(pPlayer, wndWidth, wndHeight))
		pPlayer->p = 1;

	if (pPlayer->life <= 0)
	{
		ChangeLevel(FAIL);
	}
	else if (nScore >= GetPassScore())	
	{
		switch (curLevel)
		{
		case LEVEL1:
			ChangeLevel(LEVEL2_OPEN);
			break;
		case LEVEL2:
			ChangeLevel(SUCCEED);
			break;
		}
	}
}