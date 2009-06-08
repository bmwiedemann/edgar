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

#include "random.h"
#include "../graphics/save_png.h"
#include "../map.h"

static void saveBuffer(void);
static void loadBuffer(void);

static int frame = 0;
static Input inputBuffer[MAX_INPUTS];
static int bufferID = 0;
static FILE *replayBuffer;
static int inputsRead = 0;
static char screenshotPath[MAX_PATH_LENGTH];

extern Game game;

void setReplayData(char *name)
{
	char mapFile[6];
	long seed;

	printf("Setting replay file to %s\n", name);

	replayBuffer = fopen(name, "rb");

	if (replayBuffer == NULL)
	{
		printf("Failed to open replay data %s\n", name);

		exit(1);
	}

	game.gameType = REPLAYING;

	fread(&seed, sizeof(long), 1, replayBuffer);

	printf("Setting seed %ld\n", seed);

	setSeed(seed);

	fread(mapFile, 5, 1, replayBuffer);

	mapFile[5] = '\0';

	printf("Loading map %s\n", mapFile);

	loadMap(mapFile, TRUE);
}

void setRecordData(char *name)
{
	long seed;

	printf("Setting record file to %s\n", name);

	replayBuffer = fopen(name, "wb");

	if (replayBuffer == NULL)
	{
		printf("Failed to open replay data %s\n", name);

		exit(1);
	}

	game.gameType = RECORDING;

	seed = time(NULL);

	fwrite(&seed, sizeof(long), 1, replayBuffer);

	printf("Setting seed %ld\n", seed);

	setSeed(seed);
}

void setMapFile(char *name)
{
	printf("Setting map to %s\n", name);

	fwrite(name, 5, 1, replayBuffer);
}

void setScreenshotDir(char *name)
{
	STRNCPY(screenshotPath, name, sizeof(screenshotPath));

	printf("Set screenshot directory to %s\n", screenshotPath);
}

void takeScreenshot()
{
	char filename[MAX_PATH_LENGTH];

	if (strlen(screenshotPath) != 0)
	{
		snprintf(filename, sizeof(filename), "%s/edgar%06d.bmp", screenshotPath, frame);

		frame++;

		SDL_SaveBMP(game.screen, filename);
	}
}

void takeSingleScreenshot()
{
	SDL_SaveBMP(game.screen, "edgar.bmp");
}

void putBuffer(Input inp)
{
	inputBuffer[bufferID] = inp;

	bufferID++;

	if (bufferID == MAX_INPUTS)
	{
		saveBuffer();

		bufferID = 0;
	}
}

Input getBuffer()
{
	Input inp;

	if (bufferID == 0)
	{
		loadBuffer();
	}

	if (inputsRead != MAX_INPUTS && bufferID == inputsRead)
	{
		printf("End of replay\n");

		exit(0);
	}

	inp = inputBuffer[bufferID];

	bufferID++;

	if (bufferID == MAX_INPUTS)
	{
		bufferID = 0;
	}

	return inp;
}

static void saveBuffer()
{
	fwrite(inputBuffer, sizeof(Input), bufferID, replayBuffer);
}

static void loadBuffer()
{
	inputsRead = fread(inputBuffer, sizeof(Input), MAX_INPUTS, replayBuffer);

	if (inputsRead != MAX_INPUTS)
	{
		printf("Replay buffer not completely filled. Only read %d\n", inputsRead);
	}
}

void flushBuffer(int gameType)
{
	if (gameType == RECORDING)
	{
		saveBuffer();
	}

	if (replayBuffer != NULL)
	{
		fclose(replayBuffer);
	}
}