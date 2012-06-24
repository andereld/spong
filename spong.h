#ifndef SPONG_H
#define SPONG_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include <stdbool.h>
#include <stdlib.h>

#define W_WIDTH 800		/* the width of the main window */
#define W_HEIGHT 480		/* the height of the main window */
#define W_COLOR_DEPTH 32	/* the number of bits per pixel */

#define C_WIDTH 800	/* the width of the playing court */
#define C_HEIGHT 480	/* the height of the playing court */

#define P_WIDTH 20	/* the width of a paddle */
#define P_HEIGHT 40	/* the height of a paddle */
#define P_STEP 10	/* the number of pixels a paddle moves
			   in the duration of one frame */

#define B_SIZE 20	/* the width and height of the ball */

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

typedef struct Paddle {
	SDL_Rect rect;	/* the size and position of the paddle */
	Uint32 color;	/* the color of the paddle */
} Paddle;

typedef struct Player {
	Paddle paddle;	/* a player controls a paddle ... */
	Uint8 points;	/* ... and has a number of points */
} Player;

typedef enum Direction {
	UP,
	DOWN
} Direction;

/* clear the screen to its background color */
void clearScreen(SDL_Surface *screen, Uint32 clearColor);

/* initialize and draw the playing court */
Court *makeCourt(SDL_Surface *screen);

/* initialize a player */
Player *makePlayer(SDL_Surface *screen);

/* read input from the keyboard and set the appropriate flags */
void readPlayerInput(bool *running,
		bool *player1ShouldMoveUp, bool *player1ShouldMoveDown,
		bool *player2ShouldMoveUp, bool *player2ShouldMoveDown);

/* move a player's paddle */
void movePaddle(Court *court, Paddle *paddle, Direction direction);

#endif
