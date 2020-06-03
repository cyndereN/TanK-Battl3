#ifndef _ENTITY_H_ 
#define _ENTITY_H_

enum Dir{UP, DOWN, LEFT, RIGHT, NONE};	
typedef struct	entity		
{
	int x, y;				
	Dir dir;				
	int v;					
	int s;					
	int p;					
	HBITMAP a[4];			
	int life;				
	struct entity *next;	
	unsigned short f;		
	unsigned short frames;	
	int animD;				
}Entity;


void DrawEntities(HDC hdc, const Entity *head);

void DrawEntity(HDC hdc, const Entity *ent);

void MoveEntities(Entity *head, int ts);

void Move(Entity *ent, int ts);

void Fire(const Entity* ent, Entity *pHeadB);

void Destroy(Entity *pHead, Entity* ent);

void Destroy(Entity *pHead);

int IsCollider(const Entity *ent1, const Entity *ent2);

int WallCollider(Entity *ent, int w, int h);

void NextFrameEntites(Entity *head, int ts);

void NextFrame(Entity *ent, int ts);

#endif
