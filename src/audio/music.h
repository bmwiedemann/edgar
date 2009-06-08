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

void loadMusic(char *);
void freeAllMusic(void);
void freeMusic(void);
void playMusic(void);
void playBossMusic(void);
void stopMusic(void);
void pauseMusic(void);
void loadBossMusic(char *);
void fadeOutMusic(int);
void fadeInMusic(int);
void setMusicVolume(void);
void resumeMusic(void);
void fadeBossMusic(void);