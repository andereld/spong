#ifndef SPONG_H
#define SPONG_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* COLORS */
/* Uint32s */
#define CLEAR_COLOR(format)	SDL_MapRGB(format, 0xFF, 0xFF, 0xFF)
#define WALL_COLOR(format)	SDL_MapRGB(format, 0, 0, 0)
#define PADDLE_COLOR(format)	SDL_MapRGB(format, 0, 0, 0)
#define BALL_COLOR(format)	SDL_MapRGB(format, 0, 0, 0)
/* SDL_Colors */
#define FONT_COLOR		{ r: 0, g: 0, b: 0 }

/* CONTROLS */
#define QUIT_KEY		SDLK_q
#define LPLAYER_UP_KEY		SDLK_a
#define LPLAYER_DOWN_KEY	SDLK_z
#define RPLAYER_UP_KEY		SDLK_i
#define RPLAYER_DOWN_KEY	SDLK_PERIOD

#define W_WIDTH 1280		/* the width of the main window */
#define W_HEIGHT 800		/* the height of the main window */
#define W_COLOR_DEPTH 32	/* the number of bits per pixel */

#define C_X 0			/* the leftmost coordinate of the court */
#define C_Y 0			/* the upper coordinate of the court */
#define C_WIDTH W_WIDTH		/* the width of the court */
#define C_HEIGHT W_HEIGHT	/* the height of the court */
#define C_WALL (C_HEIGHT/25)	/* the thickness of the walls */

#define P_WIDTH (C_WIDTH/60)	/* the width of a paddle */
#define P_HEIGHT (C_HEIGHT/8)	/* the height of a paddle */
#define P_STEP (C_HEIGHT/48)	/* the number of pixels a paddle moves
			   	   in the duration of one frame */

#define B_SIZE (C_WIDTH/60)	/* the width and height of the ball */
#define B_VX (C_WIDTH/80)	/* the horizontal speed of the ball */

#define FONT_SIZE (C_WIDTH/12)	/* point size */
#define F_WIDTH (C_WIDTH/8)	/* for the SDL_Surface holding the text */
#define F_HEIGHT (C_WIDTH/8)	/* for the SDL_Surface holding the text */

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
	Uint32 prevY;	/* the paddle's y-value from the previous frame;
			   used to determine whether the paddle is moving */
	Uint32 color;	/* the color of the paddle */
} Paddle;

typedef struct Player {
	Paddle paddle;	/* a player controls a paddle ... */
	Uint8 points;	/* ... and has a number of points */
} Player;

typedef enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

/* clear the screen to its background color */
void clearScreen(SDL_Surface *screen, Uint32 clearColor);

/* initialize and draw the playing court */
Court *makeCourt(SDL_Surface *screen);

/* initialize a player */
Player *makePlayer(SDL_Surface *screen);

/* initialize and draw the ball */
Ball *makeBall(SDL_Surface *screen);

/* read input from the keyboard and set the appropriate flags */
void readPlayerInput(bool *running,
		bool *lPlayerShouldMoveUp, bool *lPlayerShouldMoveDown,
		bool *rPlayerShouldMoveUp, bool *rPlayerShouldMoveDown);

/* move a player's paddle */
void movePaddle(Court *court, Paddle *paddle, Direction direction);

/* move the ball, checking for collisions and points scored */
void moveBall(Court *court, Ball *ball, Player *lPlayer, Player *rPlayer);

/* serve the ball after a point is scored */
void resetAndServe(Ball *ball, Direction direction);

/* print any errors and exit with a non-zero return status */
void die();

#endif
