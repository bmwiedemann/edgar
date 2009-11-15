/*
Copyright (C) 2009 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../headers.h"

#include "../graphics/animation.h"
#include "../audio/audio.h"
#include "../entity.h"
#include "../system/properties.h"
#include "../collisions.h"
#include "../system/error.h"
#include "../player.h"
#include "../projectile.h"
#include "../geometry.h"
#include "../world/target.h"

extern Entity *self, player;

static void hover(void);
static void redWait(void);
static void redDie(void);
static void redDieInit(void);
static void shudder(void);
static void hover(void);
static void castFireInit(void);
static void throwFire(void);
static void castFire(void);
static void castFinish(void);
static void fireDrop(void);
static void fireFall(void);
static void fireMove(void);
static void fireBounce(void);
static void fireBlock(void);
static void fireAttack(void);
static void fireAttackPause(void);

Entity *addLargeBook(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Book");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->draw = &drawLoopingAnimationToMap;
	e->touch = &entityTouch;
	e->takeDamage = &entityTakeDamageNoFlinch;
	
	if (strcmpignorecase(name, "enemy/large_red_book") == 0)
	{
		e->action = &redWait;
		e->die = &redDieInit;
	}

	e->type = ENEMY;

	setEntityAnimation(e, STAND);

	return e;
}

static void redWait()
{
	if (self->active == TRUE)
	{
		self->action = &fireAttack;
	}
	
	checkToMap(self);

	hover();
}

static void redDieInit()
{
	Target *t;
	
	self->touch = NULL;
	
	self->maxThinkTime++;
	
	switch (self->maxThinkTime)
	{
		case 1:
			t = getTargetByName("RED_BOOK_TARGET_1");
			
			self->action = &redDie;
		break;
		
		default:
			t = getTargetByName("RED_BOOK_TARGET_2");
			
			self->action = &redDie;
		break;
	}
	
	if (t == NULL)
	{
		showErrorAndExit("Red Book cannot find target");
	}
	
	self->startX = self->x;
	self->startY = 0;
	
	self->targetX = t->x;
	self->targetY = t->y;
	
	self->thinkTime = 60;
}

static void redDie()
{
	self->thinkTime--;
	
	if (self->thinkTime <= 0)
	{
		if (self->maxThinkTime == 3)
		{
			entityDie();
		}
		
		else
		{
			calculatePath(self->x, self->y, self->targetX, self->targetY, &self->dirX, &self->dirY);

			self->flags |= (NO_DRAW|HELPLESS|TELEPORTING);

			playSoundToMap("sound/common/teleport.ogg", BOSS_CHANNEL, self->x, self->y, 0);
			
			self->touch = &entityTouch;
			
			self->action = &redWait;
			
			self->active = FALSE;
			
			self->startX = self->targetX;
			self->startY = self->targetY;
			
			self->health = self->maxHealth * 1.5;
		}
	}
	
	else
	{
		shudder();
	}
}

static void fireAttack()
{
	self->thinkTime--;
	
	if (player.health > 0 && self->thinkTime <= 0)
	{
		self->action = &castFireInit;
	}
	
	facePlayer();
	
	checkToMap(self);
	
	hover();
}

static void fireAttackPause()
{
	self->thinkTime--;
	
	if (self->thinkTime <= 0)
	{
		self->action = &fireAttack;
		
		switch (self->maxThinkTime)
		{
			case 0:
				self->mental = 5;
			break;
			
			case 1:
				self->mental = 3;
			break;
			
			default:
				self->mental = 3;
			break;
		}
	}
	
	checkToMap(self);
	
	hover();	
}

static void hover()
{
	self->startX += 5;

	if (self->startX >= 360)
	{
		self->startX = 0;
	}

	self->y = self->startY + sin(DEG_TO_RAD(self->startX)) * 4;
}

static void castFireInit()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->endX = 6;

		self->action = self->maxThinkTime == 2 ? &throwFire : &castFire;

		playSoundToMap("sound/enemy/fireball/fireball.ogg", -1, self->x, self->y, 0);
	}

	hover();
}

static void throwFire()
{
	int i;
	float dir;
	Entity *e;

	self->thinkTime--;
	
	if (self->thinkTime <= 0)
	{
		for (i=0;i<5;i++)
		{
			e = getFreeEntity();

			if (e == NULL)
			{
				showErrorAndExit("No free slots to add Fire");
			}

			loadProperties("enemy/fire", e);

			e->x = self->x + self->w / 2;
			e->y = self->y + self->h / 2;

			e->x -= e->w / 2;
			e->y -= e->h / 2;
			
			dir = self->mental == 2 ? 1.50 : 1.75;
			
			e->dirY = -9;
			e->dirX = self->face == LEFT ? -(i * dir) : (i * dir);
			e->weight = 0.4;
			
			e->thinkTime = 20;
			
			e->action = &fireFall;
			e->draw = &drawLoopingAnimationToMap;
			e->touch = &entityTouch;
			e->reactToBlock = &fireBlock;
			
			e->head = self;

			e->face = self->face;

			e->type = ENEMY;

			setEntityAnimation(e, STAND);
		}
	}
	
	self->thinkTime = 90;
	
	self->action = &castFinish;
}

static void castFire()
{
	Entity *e;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		e = getFreeEntity();

		if (e == NULL)
		{
			showErrorAndExit("No free slots to add Fire");
		}

		loadProperties("enemy/fire", e);

		e->x = self->x + self->w / 2;
		e->y = self->y + self->h / 2;

		e->x -= e->w / 2;
		e->y -= e->h / 2;
		
		e->action = &fireDrop;
		e->draw = &drawLoopingAnimationToMap;
		e->touch = &entityTouch;
		e->reactToBlock = &fireBlock;
		
		e->head = self;

		e->face = self->face;

		e->type = ENEMY;

		setEntityAnimation(e, STAND);

		self->endX--;

		if (self->endX <= 0)
		{
			self->thinkTime = 30;

			self->action = &castFinish;
		}

		else
		{
			self->thinkTime = 3;
		}
	}
}

static void castFinish()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
        self->mental--;
		
		if (self->mental <= 0)
		{
			self->thinkTime = 180;
			
			self->action = &fireAttackPause;
		}
		
		else
		{
			self->action = &fireAttack;
		}
	}
}

static void fireDrop()
{
	if (self->flags & ON_GROUND)
	{
		switch (self->head->maxThinkTime)
		{
			case 0:
				self->dirX = (self->face == LEFT ? -self->speed : self->speed);
				
				self->action = &fireMove;
			break;
			
			case 1:
				self->dirX = (self->face == LEFT ? -4 : 4);
				
				self->action = &fireBounce;
			break;
		}
	}

	checkToMap(self);
}

static void fireFall()
{
	checkToMap(self);

	if (self->flags & ON_GROUND)
	{
		self->dirX = 0;
		
		self->thinkTime--;
		
		if (self->thinkTime <= 0)
		{
			self->inUse = FALSE;
		}
	}
}

static void fireMove()
{
	checkToMap(self);

	if (self->dirX == 0)
	{
		self->inUse = FALSE;
	}
}

static void fireBounce()
{
	checkToMap(self);
	
	if (self->flags & ON_GROUND)
	{
		self->dirY = -15;
	}

	if (self->dirX == 0)
	{
		self->inUse = FALSE;
	}
}

static void fireBlock()
{
	self->inUse = FALSE;
}

static void shudder()
{
	self->startY += 90;
	
	self->x = self->startX + sin(DEG_TO_RAD(self->startY)) * 4;

	if (self->startY >= 360)
	{
		self->startY = 0;
	}
}