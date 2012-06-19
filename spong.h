#ifndef SPONG_H
#define SPONG_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include <stdbool.h>
#include <stdlib.h>

#define W_WIDTH 800	/* the width of the main window */
#define W_HEIGHT 480	/* the height of the main window */

#define FPS 30				/* frames per second */
#define FRAME_DURATION ((int) 1000/FPS)	/* the duration of a frame in ms */

typedef struct Court {
	Uint16 x, y;			/* position of the upper left corner */
	Uint16 w, h;			/* width and height */
	SDL_Rect upperWall, lowerWall;	/* the walls of the court */
	Uint32 color;			/* the color of the walls */
} Court;

typedef struct Ball {
	Sint8 vx, vy;	/* the horizontal and vertical velocity of the ball */
	SDL_Rect rect;	/* the size and position of the ball */
	Uint32 color;	/* the color of the ball */
} Ball;

typedef struct Racket {
	Sint8 vy;	/* the vertical velocity of the racket;
			   a racket does not move horizontally */
	SDL_Rect rect;	/* the size and position of the racket */
	Uint32 color;	/* the color of the racket */
} Racket;

typedef struct Player {
	Racket racket;	/* a player controls a Racket ... */
	Uint8 points;	/* ... and has a number of points */
} Player;

#endif
