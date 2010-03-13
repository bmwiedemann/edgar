/*
Copyright (C) 2009-2010 Parallel Realities

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
#include "../system/properties.h"
#include "../entity.h"
#include "../map.h"
#include "../collisions.h"
#include "../item/key_items.h"
#include "../item/item.h"
#include "../system/error.h"

extern Entity *self, player;
extern Game game;

static void dropPurifierCapsule(int);
static void capsuleFallout(void);
static void purifyWater(void);
static void fillRight(int, int);
static void fillLeft(int, int);

Entity *addWaterPurifier(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Water Purifier");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->type = KEY_ITEM;

	e->action = &doNothing;
	e->touch = &keyItemTouch;
	e->activate = &dropPurifierCapsule;
	e->die = &keyItemRespawn;

	e->draw = &drawLoopingAnimationToMap;

	setEntityAnimation(e, STAND);

	return e;
}

static void dropPurifierCapsule(int val)
{
	Entity *e;

	if (self->thinkTime <= 0 && game.status == IN_GAME)
	{
		e = addTemporaryItem("item/purifier_capsule", player.x + (player.face == RIGHT ? player.w : 0), player.y + player.h / 2, player.face, player.face == LEFT ? -5 : 5, ITEM_JUMP_HEIGHT);

		e->touch = &entityTouch;
		
		e->fallout = &capsuleFallout;

		playSoundToMap("sound/common/throw.ogg", EDGAR_CHANNEL, player.x, player.y, 0);

		self->thinkTime = self->maxThinkTime;
	}
}

static void capsuleFallout()
{
	if (self->environment == SLIME)
	{
		self->action = &purifyWater;
		
		self->thinkTime = 0;
		
		self->flags |= NO_DRAW;
	}
}

static void purifyWater()
{
	fillRight(self->x / TILE_SIZE, self->y / TILE_SIZE);
	
	fillLeft((self->x / TILE_SIZE) - 1, self->y / TILE_SIZE);
	
	self->inUse = FALSE;
}

static void fillRight(int x, int y)
{
	if (mapTileAt(x, y) == SLIME_TILE_START)
	{
		setTileAt(x, y, WATER_TILE_START);
		
		x++;
		
		fillRight(x, y);
		
		x--;
		
		fillRight(x, y - 1);
		
		fillRight(x, y + 1);
	}
}

static void fillLeft(int x, int y)
{
	if (mapTileAt(x, y) == SLIME_TILE_START)
	{
		setTileAt(x, y, WATER_TILE_START);
		
		x--;
		
		fillRight(x, y);
		
		x++;
		
		fillLeft(x, y - 1);
		
		fillLeft(x, y + 1);
	}
}