/*
Copyright (C) 2009-2011 Parallel Realities

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

#include "system/pak.h"
#include "system/error.h"
#include "credits.h"
#include "inventory.h"
#include "system/properties.h"
#include "system/resources.h"
#include "graphics/animation.h"
#include "graphics/font.h"
#include "graphics/graphics.h"
#include "enemy/enemies.h"
#include "player.h"
#include "map.h"
#include "entity.h"
#include "game.h"
#include "collisions.h"
#include "item/item.h"
#include "system/load_save.h"
#include "system/random.h"
#include "system/load_save.h"

extern Game game;
extern Entity *self, entity[MAX_ENTITIES], player;

static Credits credits;
static char *enemies[] = {
			"edgar/edgar",
			"enemy/chicken",
			"enemy/grub",
			"enemy/wasp",
			"enemy/red_grub",
			"enemy/jumping_slime",
			"enemy/purple_jumping_slime",
			"enemy/red_jumping_slime",
			"enemy/scorpion",
			"enemy/snail",
			"enemy/purple_snail",
			"enemy/red_bat",
			"enemy/gazer",
			"enemy/sludge",
			"enemy/red_sludge",
			"enemy/summoner",
			"enemy/centurion",
			"enemy/red_centurion",
			"boss/centurion_boss",
			"enemy/tortoise",
			"enemy/ice_tortoise",
			"enemy/fire_tortoise",
			"enemy/ceiling_crawler",
			"enemy/large_spider",
			"enemy/large_red_spider",
			"enemy/dragon_fly"
};
static int enemiesLength = sizeof(enemies) / sizeof(char *);

static char *bosses[] = {
			"boss/grub_boss",
			"boss/golem_boss",
			"boss/snake_boss",
			"boss/blob_boss_2",
			"boss/armour_boss",
			"boss/awesome_boss_1",
			"boss/mataeus",
			"boss/phoenix",
			"boss/borer_boss",
			"boss/evil_edgar",
			"boss/sewer_boss",
			"boss/cave_boss",
			"boss/black_book_2",
			"boss/grimlore"
};
static int bossesLength = sizeof(bosses) / sizeof(char *);

static char *bossNames[] = {
			"boss/grub_boss", "King Grub",
			"boss/golem_boss", "The Golem",
			"boss/snake_boss", "The Swamp Guardian",
			"boss/blob_boss_2", "The Blob",
			"boss/armour_boss", "The Watchdog",
			"boss/awesome_boss_1", "The Awesome Foursome",
			"boss/mataeus", "Mataeus",
			"boss/phoenix", "The Phoenix",
			"boss/borer_boss", "The Borer",
			"boss/evil_edgar", "Evil Edgar",
			"boss/sewer_boss", "The Sewer Dweller",
			"boss/cave_boss", "The Salamander",
			"boss/black_book_2", "The Black Book",
			"boss/grimlore", "Grimlore"
};

static void initCredits(void);
static Entity *loadCreditsEntity(char *);
static int getNextEntity(void);
static void doEndCredits(void);
static void doGameStats(void);
static void initGameStats(void);
static void drawEndCredits(void);
static void drawGameStats(void);
static void doDefeatedBosses(void);
static void initDefeatedBosses(void);
static int getNextBoss(void);
static void drawDefeatedBosses(void);
static Entity *loadCreditsBoss(char *);
static void bossMoveToMiddle(void);
static void bossMoveOffScreen(void);
static char *getBossName(char *);

void doCredits()
{
	switch (credits.status)
	{
		case 1:
			doGameStats();
		break;
		
		case 2:
			doDefeatedBosses();
		break;
		
		default:
			doEndCredits();
		break;
	}
}

static void doDefeatedBosses()
{
	int i, remainingEntities;
	
	if (credits.creditLine == NULL)
	{
		initDefeatedBosses();
	}
	
	remainingEntities = 0;
	
	for (i=0;i<MAX_ENTITIES;i++)
	{
		self = &entity[i];

		if (self->inUse == TRUE)
		{
			remainingEntities++;
			
			self->action();
			
			addToGrid(self);
			
			addToDrawLayer(self, self->layer);
		}
	}
	
	if (remainingEntities == 0)
	{
		if (getNextBoss() == FALSE)
		{
			credits.fading = TRUE;
		}
		
		else
		{
			setMapStartX(0);
			setMapStartY(0);
		}
	}
	
	if (credits.fading == TRUE)
	{
		fadeCredits();
		
		credits.alpha++;
		
		if (credits.alpha == 255)
		{
			freeCredits();
			
			player.inUse = FALSE;
			
			freeEntities();
			
			credits.line = 0;
			credits.lineCount = 0;
			credits.entityID = 0;
			credits.fading = FALSE;
			credits.alpha = 255;
			credits.startDelay = 0;
			credits.nextEntityDelay = 0;
			
			newGame();
		}
	}
}

static void doGameStats()
{
	if (credits.creditLine == NULL)
	{
		initGameStats();
	}
	
	credits.startDelay--;
	
	if (credits.startDelay <= 0)
	{
		if (credits.fading == TRUE)
		{
			freeCredits();
			
			player.inUse = FALSE;
			
			freeEntities();
			
			credits.line = 0;
			credits.lineCount = 0;
			credits.entityID = 0;
			credits.fading = FALSE;
			credits.alpha = 255;
			credits.startDelay = 0;
			credits.nextEntityDelay = 0;
			
			credits.status = 2;
		}
		
		else
		{
			credits.line += credits.line == 0 ? 1 : 2;
			
			if (credits.line > credits.lineCount)
			{
				credits.fading = TRUE;
				
				credits.line = credits.lineCount;
			}
			
			credits.startDelay = credits.line == credits.lineCount ? 180 : 60;
		}
	}
}

static void doEndCredits()
{
	int i, r, g, b, remainingEntities;
	
	if (credits.creditLine == NULL)
	{
		initCredits();
	}
	
	credits.logoY -= 0.25;
	
	if (credits.logoY < -SCREEN_HEIGHT)
	{
		SDL_FreeSurface(credits.logo);
		
		credits.logo = NULL;
	}
	
	remainingEntities = 0;
	
	for (i=0;i<credits.lineCount;i++)
	{
		credits.creditLine[i].y -= 0.25;
		
		if (credits.creditLine[i].y < -SCREEN_HEIGHT)
		{
			SDL_FreeSurface(credits.creditLine[i].textImage);
			
			credits.creditLine[i].textImage = NULL;
		}
		
		else
		{
			remainingEntities++;
			
			if (credits.creditLine[i].y <= SCREEN_HEIGHT)
			{
				if (credits.creditLine[i].textImage == NULL && credits.creditLine[i].text[0] != '#')
				{
					r = credits.creditLine[i].r;
					g = credits.creditLine[i].g;
					b = credits.creditLine[i].b;
					
					credits.creditLine[i].textImage = generateTransparentTextSurface(credits.creditLine[i].text, game.font, r, g, b, TRUE);
				}
			}
		}
	}
	
	credits.startDelay--;
	
	if (credits.startDelay <= 0)
	{
		credits.startDelay = 0;
		
		if (player.inUse == TRUE)
		{
			remainingEntities++;
			
			self = &player;
	
			self->dirY += GRAVITY_SPEED * self->weight;

			if (self->dirY >= MAX_AIR_SPEED)
			{
				self->dirY = MAX_AIR_SPEED;
			}

			else if (self->dirY > 0 && self->dirY < 1)
			{
				self->dirY = 1;
			}
			
			self->creditsAction();
			
			addToGrid(self);
		}
		
		for (i=0;i<MAX_ENTITIES;i++)
		{
			self = &entity[i];

			if (self->inUse == TRUE)
			{
				if (!(self->flags & TELEPORTING))
				{
					remainingEntities++;
					
					if (!(self->flags & (FLY|GRABBED)))
					{
						switch (self->environment)
						{
							case WATER:
							case SLIME:
								self->dirY += GRAVITY_SPEED * 0.25 * self->weight;

								if (self->flags & FLOATS)
								{
									if (self->dirX != 0)
									{
										self->endY++;

										self->dirY = cos(DEG_TO_RAD(self->endY)) / 20;
									}
								}

								if (self->dirY >= MAX_WATER_SPEED)
								{
									self->dirY = MAX_WATER_SPEED;
								}
							break;

							default:
								self->dirY += GRAVITY_SPEED * self->weight;

								if (self->dirY >= MAX_AIR_SPEED)
								{
									self->dirY = MAX_AIR_SPEED;
								}

								else if (self->dirY > 0 && self->dirY < 1)
								{
									self->dirY = 1;
								}
							break;
						}
					}
					
					if (self->creditsAction == NULL)
					{
						showErrorAndExit("%s has no Credits Action defined", self->name);
					}
					
					self->creditsAction();
					
					addToGrid(self);
					
					addToDrawLayer(self, self->layer);
				}
				
				else
				{
					doTeleport();
				}
			}
		}
		
		credits.nextEntityDelay--;
		
		if (credits.nextEntityDelay <= 0)
		{
			if (getNextEntity() == FALSE && remainingEntities == 0)
			{
				credits.fading = TRUE;
			}
			
			setMapStartX(TILE_SIZE * 4);
			
			centerMapOnEntity(NULL);
			
			credits.nextEntityDelay = 300;
		}
	}
	
	if (credits.fading == TRUE)
	{
		fadeCredits();
		
		credits.alpha++;
		
		if (credits.alpha == 255)
		{
			freeCredits();
			
			player.inUse = FALSE;
			
			freeEntities();
			
			credits.line = 0;
			credits.lineCount = 0;
			credits.entityID = 0;
			credits.fading = FALSE;
			credits.alpha = 255;
			credits.startDelay = 0;
			credits.nextEntityDelay = 0;
			
			credits.status = 1;
		}
	}
}

void drawCredits()
{
	switch (credits.status)
	{
		case 1:
			drawGameStats();
		break;
		
		case 2:
			drawDefeatedBosses();
		break;
		
		default:
			drawEndCredits();
		break;
	}
}

static void drawDefeatedBosses()
{
	int i;
	
	if (credits.creditLine != NULL)
	{
		for (i=0;i<credits.lineCount;i++)
		{
			if (credits.creditLine[i].textImage != NULL)
			{
				drawImage(credits.creditLine[i].textImage, (SCREEN_WIDTH - credits.creditLine[i].textImage->w) / 2, credits.creditLine[i].y, FALSE, 255);
			}
		}
	}
}

static void drawGameStats()
{
	int i;
	
	if (credits.creditLine != NULL)
	{
		for (i=0;i<credits.line;i++)
		{
			drawImage(credits.creditLine[i].textImage, credits.creditLine[i].x, credits.creditLine[i].y, FALSE, 255);
		}
	}
}

static void drawEndCredits()
{
	int i;
	
	if (credits.logo != NULL)
	{
		drawImage(credits.logo, (SCREEN_WIDTH - credits.logo->w) / 2, credits.logoY, FALSE, 255);
	}	
	
	for (i=0;i<credits.lineCount;i++)
	{
		if (credits.creditLine[i].textImage != NULL)
		{
			drawImage(credits.creditLine[i].textImage, (SCREEN_WIDTH - credits.creditLine[i].textImage->w) / 2, credits.creditLine[i].y, FALSE, 255);
		}
	}
	
	if (credits.fading == TRUE)
	{
		drawImage(credits.fadeSurface, 0, 0, FALSE, credits.alpha);
	}
}

static void initGameStats()
{
	int i, x, y;
	
	credits.lineCount = 15;
	
	credits.creditLine = malloc(credits.lineCount * sizeof(CreditLine));

	if (credits.creditLine == NULL)
	{
		showErrorAndExit("Failed to allocate %d bytes for end credits...", credits.lineCount * sizeof(CreditLine));
	}
	
	STRNCPY(credits.creditLine[0].text, "Statistics", MAX_LINE_LENGTH);
	
	snprintf(credits.creditLine[1].text, MAX_LINE_LENGTH, "Play Time");
	snprintf(credits.creditLine[2].text, MAX_LINE_LENGTH, "%s", getPlayTimeAsString());
	
	snprintf(credits.creditLine[3].text, MAX_LINE_LENGTH, "Kills");
	snprintf(credits.creditLine[4].text, MAX_LINE_LENGTH, "%d", game.kills);
	
	snprintf(credits.creditLine[5].text, MAX_LINE_LENGTH, "Arrows Fired");
	snprintf(credits.creditLine[6].text, MAX_LINE_LENGTH, "%d", game.arrowsFired);
	
	snprintf(credits.creditLine[7].text, MAX_LINE_LENGTH, "Distanced Travelled");
	snprintf(credits.creditLine[8].text, MAX_LINE_LENGTH, "%ud", game.distanceTravelled / 45000);
	
	snprintf(credits.creditLine[7].text, MAX_LINE_LENGTH, "Attacks Blocked");
	snprintf(credits.creditLine[8].text, MAX_LINE_LENGTH, "%d", game.attacksBlocked);
	
	snprintf(credits.creditLine[9].text, MAX_LINE_LENGTH, "Time Spent As A Slime");
	snprintf(credits.creditLine[10].text, MAX_LINE_LENGTH, "%s", getSlimeTimeAsString());
	
	snprintf(credits.creditLine[11].text, MAX_LINE_LENGTH, "Secrets Found");
	snprintf(credits.creditLine[12].text, MAX_LINE_LENGTH, _("%d / %d"), game.secretsFound, TOTAL_SECRETS);
	
	snprintf(credits.creditLine[13].text, MAX_LINE_LENGTH, "Continues");
	snprintf(credits.creditLine[14].text, MAX_LINE_LENGTH, "%d", game.continues);
	
	y = 32;
	
	x = 0;
	
	for (i=0;i<credits.lineCount;i++)
	{
		credits.creditLine[i].textImage = generateTransparentTextSurface(credits.creditLine[i].text, game.largeFont, 220, 220, 220, TRUE);
		
		if (credits.creditLine[i].textImage->w > x)
		{
			x = credits.creditLine[i].textImage->w;
		}
	}
	
	for (i=0;i<credits.lineCount;i++)
	{
		if (i == 0)
		{
			credits.creditLine[i].x = (SCREEN_WIDTH - credits.creditLine[i].textImage->w) / 2;
			credits.creditLine[i].y = y;
			
			y += credits.creditLine[i].textImage->h + 16;
		}
		
		else if (i % 2 == 1)
		{
			credits.creditLine[i].x = 64;
			credits.creditLine[i].y = y;
		}
		
		else
		{
			credits.creditLine[i].x = credits.creditLine[i - 1].x + x + 64;
			credits.creditLine[i].y = y;
			
			y += credits.creditLine[i].textImage->h + 16;
		}
	}
	
	credits.startDelay = 60;
}

static void initCredits()
{
	int lineNum, y;
	char *buffer, *token1, *token2, *savePtr1, *savePtr2;
	
	credits.logo = loadImage("gfx/title_screen/logo.png");
	
	buffer = (char *)loadFileFromPak("data/credits");
	
	credits.lineCount = countTokens(buffer, "\n");
	
	credits.creditLine = malloc(credits.lineCount * sizeof(CreditLine));
	
	if (credits.creditLine == NULL)
	{
		showErrorAndExit("Failed to allocate %d bytes for end credits...", credits.lineCount * sizeof(CreditLine));
	}
	
	lineNum = 0;
	
	token1 = strtok_r(buffer, "\n", &savePtr1);
	
	y = SCREEN_HEIGHT;
	
	credits.logoY = y;
	
	y += credits.logo->h + 32;
	
	while (token1 != NULL)
	{
		token2 = strtok_r(token1, " ", &savePtr2);
		
		credits.creditLine[lineNum].r = atoi(token2);
		
		token2 = strtok_r(NULL, " ", &savePtr2);
		
		credits.creditLine[lineNum].g = atoi(token2);
		
		token2 = strtok_r(NULL, " ", &savePtr2);
		
		credits.creditLine[lineNum].b = atoi(token2);
		
		token2 = strtok_r(NULL, "\0", &savePtr2);
		
		STRNCPY(credits.creditLine[lineNum].text, token2, MAX_LINE_LENGTH);
		
		credits.creditLine[lineNum].y = y;
		
		credits.creditLine[lineNum].textImage = NULL;
		
		y += 32;
		
		lineNum++;
		
		token1 = strtok_r(NULL, "\n", &savePtr1);
	}
	
	free(buffer);
	
	saveTemporaryData();
	
	freeInventory();

	freeLevelResources();
	
	loadMap("map_credits", TRUE);
	
	setMapStartX(TILE_SIZE * 4);
	
	credits.entityID = 0;
	
	player.inUse = FALSE;
	
	credits.startDelay = 300;
}

static void initDefeatedBosses()
{
	credits.lineCount = 3;
	
	credits.creditLine = malloc(credits.lineCount * sizeof(CreditLine));

	if (credits.creditLine == NULL)
	{
		showErrorAndExit("Failed to allocate %d bytes for end credits...", credits.lineCount * sizeof(CreditLine));
	}
	
	STRNCPY(credits.creditLine[0].text, _("Defeated Bosses"), MAX_LINE_LENGTH);
	
	credits.creditLine[0].y = 64;
	
	credits.creditLine[0].textImage = generateTransparentTextSurface(credits.creditLine[0].text, game.largeFont, 220, 220, 220, TRUE);
	
	STRNCPY(credits.creditLine[1].text, "", MAX_LINE_LENGTH);
	
	credits.creditLine[1].y = 350;
	
	credits.creditLine[1].textImage = NULL;
	
	STRNCPY(credits.creditLine[2].text, "", MAX_LINE_LENGTH);
	
	credits.creditLine[2].y = 400;
	
	credits.creditLine[2].textImage = NULL;
	
	credits.startDelay = 60;
}

int countTokens(char *line, char *delim)
{
	char *temp, *savePtr, *token;
	int i;
	
	temp = malloc(strlen(line) + 1);
	
	if (temp == NULL)
	{
		showErrorAndExit("Failed to allocate a whole %d bytes for tokens...", (int)strlen(line) + 1);
	}
	
	STRNCPY(temp, line, strlen(line) + 1);

	token = strtok_r(temp, delim, &savePtr);
	
	i = 0;

	while (token != NULL)
	{
		i++;
		
		token = strtok_r(NULL, delim, &savePtr);
	}
	
	free(temp);
	
	return i;
}

void freeCredits()
{
	int i;
	
	if (credits.creditLine != NULL)
	{
		for (i=0;i<credits.lineCount;i++)
		{
			if (credits.creditLine[i].textImage != NULL)
			{
				SDL_FreeSurface(credits.creditLine[i].textImage);
				
				credits.creditLine[i].textImage = NULL;
			}
		}
		
		free(credits.creditLine);
		
		credits.creditLine = NULL;
	}
	
	if (credits.logo != NULL)
	{
		SDL_FreeSurface(credits.logo);
		
		credits.logo = NULL;
	}
	
	if (credits.fadeSurface != NULL)
	{
		SDL_FreeSurface(credits.fadeSurface);
		
		credits.fadeSurface = NULL;
	}
}

static Entity *loadCreditsEntity(char *name)
{
	Entity *e;
	
	if (strstr(name, "/edgar") != NULL)
	{
		e = loadPlayer(0, 0, name);
		
		addPermanentItem("weapon/fire_shield", 0, 300);
		
		addPermanentItem("weapon/basic_sword", 0, 300);
	}
	
	else
	{
		e = addEnemy(name, 0, 0);
	}
	
	e->x = 0;
	
	e->y = 250;
	
	e->face = RIGHT;
	
	e->startX = e->x;
	e->startY = e->y;
	
	return e;
}

static int getNextEntity()
{
	if (credits.entityID >= enemiesLength)
	{
		return FALSE;
	}
	
	loadCreditsEntity(enemies[credits.entityID]);
	
	credits.entityID++;
	
	return TRUE;
}

static Entity *loadCreditsBoss(char *name)
{
	Entity *e;
	
	e = addEnemy(name, SCREEN_WIDTH, 0);
	
	e->y = (SCREEN_HEIGHT - e->h) / 2;
	
	e->flags &= ~NO_DRAW;
	
	e->action = &bossMoveToMiddle;
	
	e->targetX = (SCREEN_WIDTH - e->w) / 2;
	
	e->thinkTime = 120;
	
	printf("Added enemy %s\n", e->name);
	
	STRNCPY(credits.creditLine[1].text, getBossName(e->name), MAX_LINE_LENGTH);
	
	STRNCPY(credits.creditLine[2].text, bossExists(e->name) == TRUE ? "" : "Defeated", MAX_LINE_LENGTH);
		
	if (credits.creditLine[1].textImage != NULL)
	{
		SDL_FreeSurface(credits.creditLine[1].textImage);
		
		credits.creditLine[1].textImage = NULL;
	}
	
	if (credits.creditLine[2].textImage != NULL)
	{
		SDL_FreeSurface(credits.creditLine[2].textImage);
		
		credits.creditLine[2].textImage = NULL;
	}
	
	credits.creditLine[1].textImage = generateTransparentTextSurface(credits.creditLine[1].text, game.largeFont, 220, 220, 220, TRUE);
	
	return e;
}

static int getNextBoss()
{
	if (credits.entityID >= bossesLength)
	{
		return FALSE;
	}
	
	loadCreditsBoss(bosses[credits.entityID]);
	
	credits.entityID++;
	
	return TRUE;
}

void fadeCredits()
{
	if (credits.fadeSurface == NULL)
	{
		credits.fading = TRUE;
		
		credits.alpha = 0;
		
		credits.fadeSurface = createSurface(game.screen->w, game.screen->h);

		drawBox(credits.fadeSurface, 0, 0, game.screen->w, game.screen->h, 0, 0, 0);
	}
}

static void bossMoveToMiddle()
{
	if (self->x <= self->targetX)
	{
		if (strlen(credits.creditLine[2].text) != 0 && credits.creditLine[2].textImage == NULL)
		{
			credits.creditLine[2].textImage = generateTransparentTextSurface(credits.creditLine[2].text, game.largeFont, 0, 220, 0, TRUE);
		}
		
		self->thinkTime--;
		
		if (self->thinkTime <= 0)
		{
			self->action = &bossMoveOffScreen;
		}
	}
	
	else
	{
		self->x -= 20;
	}
}

static void bossMoveOffScreen()
{
	self->x -= 20;
	
	if (self->x <= -self->w)
	{
		self->inUse = FALSE;
	}
}

static char *getBossName(char *name)
{
	int i;
	
	printf("Searching for %s\n", name);
	
	for (i=0;i<bossesLength*2;i+=2)
	{
		printf("'%s' == '%s'\n", name, bossNames[i]);
		
		if (strcmpignorecase(name, bossNames[i]) == 0)
		{
			return bossNames[i + 1];
		}
	}
	
	showErrorAndExit("Could not find name for boss %s", name);
	
	return NULL;
}