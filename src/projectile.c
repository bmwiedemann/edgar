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

#include "headers.h"

#include "graphics/animation.h"
#include "system/properties.h"
#include "entity.h"
#include "collisions.h"

static void projectileMove(void);

extern Entity *self;

Entity *addProjectile(char *name, Entity *owner, int x, int y, float dirX, float dirY)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		printf("No free slots to add a projectile\n");

		exit(1);
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->dirX = dirX;
	e->dirY = dirY;

	e->action = &projectileMove;

	e->draw = &drawLoopingAnimationToMap;

	e->touch = &entityTouch;

	e->type = PROJECTILE;

	e->parent = owner;

	setEntityAnimation(e, STAND);

	return e;
}

static void projectileMove()
{
	self->face = self->dirX > 0 ? RIGHT : LEFT;
	
	self->thinkTime--;
	
	if (self->thinkTime <= 0)
	{
		self->inUse = FALSE;
	}

	checkToMap(self);
}

void bounceOffShield()
{
	self->dirX *= -1;

	self->flags &= ~FLY;

	self->touch = NULL;
}