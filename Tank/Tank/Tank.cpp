// Tank.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Tank.h"
#include "time.h"

#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

enum Dir { UP, DOWN, LEFT, RIGHT };	// 4 directions objects may go for
typedef struct				// Tank & Bullet
{
	int x, y;				// Position
	Dir dir;				// Direction
	int v;					// Velocity
	int s;					// Length of side
	int b;					// is Bullet?
	int p;					// is Stop? (only player)
	int e;					// is Enemy?
	COLORREF c;				// Color
}Entity;

#define MAX_ENEMY 16		// Maximum number of enemies
#define MAX_BULLETS 32		// Maximum number of bullets
int nLife = 3;				// Player HP
int nScore = 0;				// Player score
int nBullet = 0;			// Number of player bullet
int nEnemyBullet = 0;		// Number of enemy bullet
int nEnemy = 0;				// Number of existing enemies
int timeStep = 20;			// Timer interval
int sz = 50;				// Size of tank
int velf = 4;				// Velocity of fast enemy tank
int vels = 2;				// Velocity of slow enemy tank
int szb = 20;				// Size of bullet
int velb = 6;				// Velocity of bullet
int enemyFirePer = 150;		// Random firing percentage of enemy
int enemyDir = 100;			// Random direction changing percentage of enemy 
int bFire = 0;				// is Player firing?
Entity enemies[MAX_ENEMY];	// Enemy struct array
Entity player;				// Player struct
Entity bullets[MAX_BULLETS];// Player bullet struct array
Entity enemyBullets[MAX_BULLETS];// Enemy bullet struct array
int wndWidth = 0;	int wndHeight = 0;  // Size of window

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Reset player
void ResetPlayer()
{
	player.s = sz;
	player.b = 0;
	player.c = RGB(122, 30, 0);
	player.dir = UP;
	player.v = vels;
	player.x = wndWidth / 2;
	player.y = wndHeight - sz/2;
	player.p = 1;
	player.e = 0;
}

// Game initializing
void Init()
{
	for (nEnemy = 0; nEnemy < MAX_ENEMY; nEnemy++)
	{
		enemies[nEnemy].s = sz;
		enemies[nEnemy].b = 0;
		enemies[nEnemy].e = 1;
		enemies[nEnemy].dir = Dir(rand() % 4);		// Randomly pick a direction
		enemies[nEnemy].v = rand() % 2 == 0 ? velf : vels;	// Randomly pick a velocity of enemy tank
		enemies[nEnemy].c = enemies[nEnemy].v == velf ? RGB(0, 122, 122) : RGB(0, 60, 30);	
		enemies[nEnemy].x = (rand() % 3) * (wndWidth - sz) / 2 + sz / 2;	// Randomly pick a born point
		enemies[nEnemy].y = sz;
		enemies[nEnemy].p = 0;
	}
	ResetPlayer();
}

// Move secific object
void Move(Entity* ent, int ts) // Struct pointer
{
	if (ent->p)
		return;
	switch (ent->dir)
	{
	case UP:
		ent->y -= ent->v * ts;
		break;
	case DOWN:
		ent->y += ent->v * ts;
		break;
	case LEFT:
		ent->x -= ent->v * ts;
		break;
	case RIGHT:
		ent->x += ent->v * ts;
		break;
	}
}

// Specific object firing
void Fire(const Entity* ent)
{
	Entity* pBullets = (ent->e) ? enemyBullets : bullets;	// is Enemy or Player firing?
	int nB = (ent->e) ? nEnemyBullet : nBullet;
	if (nB >= MAX_BULLETS)
		return;
	(pBullets + nB)->s = szb;
	(pBullets + nB)->b = 1;
	(pBullets + nB)->e = 0;
	(pBullets + nB)->c = (ent->e) ? RGB(0, 0, 255) : RGB(255, 0, 0);	// Different bullet between Enemy and Player
	(pBullets + nB)->dir = ent->dir;
	(pBullets + nB)->v = velb;
	(pBullets + nB)->x = ent->x; // Bullet position is Object position
	(pBullets + nB)->p = 0;
	(pBullets + nB)->y = ent->y;
	switch (ent->dir)	// Bullet direction is same as object direction
	{
	case UP:
		(pBullets + nB)->y -= ent->s;
		break;
	case DOWN:
		(pBullets + nB)->y += ent->s;
		break;
	case LEFT:
		(pBullets + nB)->x -= ent->s;
		break;
	case RIGHT:
		(pBullets + nB)->x += ent->s;
		break;
	}
	if (ent->e)
		nEnemyBullet++;
	else
		nBullet++;
}

// Destroy certain object and move the following ones forward
void Destroy(Entity ents[], int n, int* num)
{
	memcpy(ents + n, ents + n + 1, sizeof(Entity) * ((*num) - 1 - n)); // Move the following ones forward
	(*num)--; 
}

// is Collide?
int IsCollider(const Entity* ent1, const Entity* ent2)
{

	if (ent1->x + ent1->s / 2 <= ent2->x - ent2->s / 2 || ent1->x - ent1->s / 2 >= ent2->x + ent2->s / 2)
		return 0;
	if (ent1->y + ent1->s / 2 <= ent2->y - ent2->s / 2 || ent1->y - ent1->s / 2 >= ent2->y + ent2->s / 2)
		return 0;
	return 1;
}

// is Collide against wall?
int WallCollider(Entity* ent)
{
	int bC = 0;
	switch (ent->dir)
	{
	case UP:
		if ((ent->y - ent->s / 2) < 0)			// Upper border
		{
			bC = 1;
			ent->y = ent->s / 2;
		}
		break;
	case DOWN:
		if ((ent->y + ent->s / 2) > wndHeight)	// Lower border
		{
			bC = 1;
			ent->y = wndHeight - ent->s / 2;
		}
		break;
	case LEFT:
		if ((ent->x - ent->s / 2) < 0)				// Left border
		{
			bC = 1;
			ent->x = ent->s / 2;
		}
		break;
	case RIGHT:
		if ((ent->x + ent->s / 2) > wndWidth)		// Right border
		{
			bC = 1;
			ent->x = wndWidth - ent->s / 2;
		}
		break;
	}
	if (bC)
	{
		if (ent->e)				// If enemy collide against wall
			ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
		else					// If enemy collide against wall
			ent->p = 1;         // Player stops
	}
	return bC;
}

// Update the game, call in "Timer"
void Update(int ts)
{
	// Update movable objects
	Entity* ent = NULL;

	for (int i = 0; i < nEnemy; i++)	// Update enemy position
	{
		ent = enemies + i;
		Move(ent, ts);
		if ((rand() % enemyFirePer) == 0)
			Fire(ent);
	}
	for (int i = 0; i < nBullet; i++)	// Update bullet position
	{
		ent = bullets + i;
		Move(ent, ts);
	}
	for (int i = 0; i < nEnemyBullet; i++)	// Update enemy bullet position
	{
		ent = enemyBullets + i;
		Move(ent, ts);
	}
	Move(&player, ts);					// Update player position

	if (bFire)							// Update player bullet position
	{
		Fire(&player);
		bFire = 0;
	}

	// Is Player Bullet collide against Enemy?
	for (int i = 0; i < nBullet; i++)
	{
		for (int j = 0; j < nEnemy; j++)
		{
			if (IsCollider(&bullets[i], &enemies[j]))
			{
				Destroy(bullets, i, &nBullet);
				Destroy(enemies, j, &nEnemy);
				nScore++;
				i--;
				j--;
				break;
			}
		}
	}
	// Is Enemy Bullet collide against Player?
	for (int i = 0; i < nEnemyBullet; i++)
	{
		if (IsCollider(&enemyBullets[i], &player))
		{
			Destroy(enemyBullets, i, &nEnemyBullet);
			ResetPlayer();
			nLife--;
			i--;
			break;
		}
	}
	// Is Enemy collide against Player?
	for (int i = 0; i < nEnemy; i++)
	{
		if (IsCollider(&player, &enemies[i]))
		{
			ResetPlayer();
			nLife--;
		}
	}
	// Is objects collide against Wall?
	for (int i = 0; i < nEnemy; i++)	// Enemy
	{
		ent = enemies + i;
		if (!WallCollider(ent))		// Has possibility to change direction
		{
			if (rand() % enemyDir == 0)
				ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
		}
	}
	for (int i = 0; i < nBullet; i++)	// Player Bullet
	{
		ent = bullets + i;
		if (WallCollider(ent))
		{
			Destroy(bullets, i, &nBullet);
			i--;
		}
	}
	for (int i = 0; i < nEnemyBullet; i++)	// Enemy Bullet
	{
		ent = enemyBullets + i;
		if (WallCollider(ent))
		{
			Destroy(enemyBullets, i, &nEnemyBullet);
			i--;
		}
	}
	WallCollider(&player);			// Player
}
// Draw objects
void DrawEntity(HDC hdc, const Entity* ent)
{
	HBRUSH brush;
	brush = CreateSolidBrush(ent->c);		// Create brush according to color
	RECT rc;								// rectangle
	rc.top = ent->y - ent->s / 2;
	rc.left = ent->x - ent->s / 2;
	rc.bottom = ent->y + ent->s / 2;
	rc.right = ent->x + ent->s / 2;
	FillRect(hdc, &rc, brush);				// Draw 
	if (!ent->b)							// If not bullet, draw barrel
	{
		switch (ent->dir)
		{
		case UP:
			rc.bottom = rc.top;
			rc.top = rc.bottom - ent->s / 2;
			rc.left = rc.left + ent->s / 4;
			rc.right = rc.right - ent->s / 4;
			break;
		case DOWN:
			rc.top = rc.bottom;
			rc.bottom = rc.bottom + ent->s / 2;
			rc.left = rc.left + ent->s / 4;
			rc.right = rc.right - ent->s / 4;
			break;
		case LEFT:
			rc.right = rc.left;
			rc.left = rc.left - ent->s / 2;
			rc.bottom = rc.bottom - ent->s / 4;
			rc.top = rc.top + ent->s / 4;
			break;
		case RIGHT:
			rc.left = rc.right;
			rc.right = rc.right + ent->s / 2;
			rc.bottom = rc.bottom - ent->s / 4;
			rc.top = rc.top + ent->s / 4;
			break;
		}
		FillRect(hdc, &rc, brush);
	}
	DeleteObject(brush);			// Delete brush
}

// Draw scene 
void DrawScene(HDC hdc)
{
	// Draw hints message
	HFONT hf;
	WCHAR str[32];
	long lfHeight;
	lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	HFONT hfOld = (HFONT)SelectObject(hdc, hf);
	if (nLife <= 0)							
	{
		SetTextColor(hdc, RGB(122, 0, 0));
		TextOut(hdc, wndWidth / 2 - 50, wndHeight / 2 - 40, L"Game Over", 9);
		SelectObject(hdc, hfOld);
		return;
	}
	// Statistical data
	SetTextColor(hdc, RGB(100, 100, 100));
	wsprintf(str, L"Life:%d Score:%d", nLife, nScore);
	TextOut(hdc, wndWidth / 2 - 50, wndHeight / 2 - 40, str, wcslen(str));

	SelectObject(hdc, hfOld);				// default font
	DeleteObject(hf);

	// Draw objects
	Entity* ent = NULL;
	for (int i = 0; i < nEnemy; i++)		// Enemy
	{
		ent = enemies + i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nBullet; i++)		// Player Bullets
	{
		ent = bullets + i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nEnemyBullet; i++)	// Enemy Bullets
	{
		ent = enemyBullets + i;
		DrawEntity(hdc, ent);
	}

	DrawEntity(hdc, &player);				// Player
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TANK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TANK));

	srand(time(NULL));		// Generate random seed
	Init();					// Initializing

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TANK));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TANK);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:			// Timer
		SetTimer(hWnd, 1, timeStep, NULL);
		break;
	case WM_TIMER:			// Timer reaction
		if (wParam == 1)	
		{
			if (nLife > 0)
				Update(timeStep / 10);
			InvalidateRect(hWnd, NULL, TRUE);	// Redraw
		}
		break;
	case WM_SIZE:			// Obatain the size of window
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		break;
	case WM_KEYDOWN:		// Key down
	{
		InvalidateRect(hWnd, NULL, TRUE);
		switch (wParam)	
		{
		case VK_LEFT:
			player.dir = LEFT;
			player.p = 0;
			break;
		case VK_RIGHT:
			player.dir = RIGHT;
			player.p = 0;
			break;
		case VK_UP:
			player.dir = UP;
			player.p = 0;
			break;
		case VK_DOWN:
			player.dir = DOWN;
			player.p = 0;
			break;
		case VK_SPACE:	// Fire
			bFire = 1;;
			break;
		}
		break;
	}
	case WM_ERASEBKGND:		// Don't wipe background to avoid refresh 
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		// To avoid refresh, draw next scene in memory and copyto screen
		// Create memory hdc
		HDC memHDC = CreateCompatibleDC(hdc);

		// Obtain size of client
		RECT rectClient;
		GetClientRect(hWnd, &rectClient);

		// Create bmp
		HBITMAP bmpBuff = CreateCompatibleBitmap(hdc, wndWidth, wndHeight);
		HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
		PatBlt(memHDC, 0, 0, wndWidth, wndHeight, WHITENESS);	// White background

		// Draw scene
		DrawScene(memHDC);

		// Copy memhdc to hdc
		BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, wndWidth,
			wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

		// Recover Memory
		SelectObject(memHDC, pOldBMP);
		DeleteObject(bmpBuff);
		DeleteDC(memHDC);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		KillTimer(hWnd, 1);		// Kill Timer
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}