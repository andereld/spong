#include "spong.h"

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	/* the frame buffer */
	SDL_Surface *screen = SDL_SetVideoMode(W_WIDTH, W_HEIGHT,
			W_COLOR_DEPTH, SDL_SWSURFACE);

	/* the background color of the screen */
	const Uint32 clearColor = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	clearScreen(screen, clearColor);

	/* set up the playing court */
	Court *court = makeCourt(screen);

	/* set up the players and their paddles */
	Player *player1 = makePlayer(screen);
	Player *player2 = makePlayer(screen);
	player2->paddle.rect.x = C_WIDTH - P_WIDTH;

	/* set up the player controls; these need to be #defines to satisfy
	   the compiler's idea of an 'integer constant' in a switch statement */
#define P1_UP_KEY SDLK_a
#define P1_DOWN_KEY SDLK_z
#define P2_UP_KEY SDLK_i
#define P2_DOWN_KEY SDLK_PERIOD
	/* because SDL_KEY(UP|DOWN) only occur once, not continuously while
	   the key is pressed, we need to keep track of whether a key is
	   (still) pressed */
	bool player1ShouldMoveUp = false, player1ShouldMoveDown = false,
	     player2ShouldMoveUp = false, player2ShouldMoveDown = false;

	Uint32 startTime;	/* denotes the beginning of each iteration
				   of the main event loop */
	bool running = true;	/* true till the application should exit */
	while (running) {
		startTime = SDL_GetTicks();

		/* overwrite the previous frame's paddles */
		SDL_FillRect(screen, &player1->paddle.rect, clearColor);
		SDL_FillRect(screen, &player2->paddle.rect, clearColor);

		/* get the current state of the players' controls */
		readPlayerInput(&running,
				&player1ShouldMoveUp, &player1ShouldMoveDown,
				&player2ShouldMoveUp, &player2ShouldMoveDown);

		/* move the paddles if appropriate */
		if (player1ShouldMoveUp)
			movePaddle(court, &player1->paddle, UP);
		else if (player1ShouldMoveDown)
			movePaddle(court, &player1->paddle, DOWN);
		if (player2ShouldMoveUp)
			movePaddle(court, &player2->paddle, UP);
		else if (player2ShouldMoveDown)
			movePaddle(court, &player2->paddle, DOWN);

		/* draw the paddles */
		SDL_FillRect(screen, &player1->paddle.rect,
				player1->paddle.color);
		SDL_FillRect(screen, &player2->paddle.rect,
				player2->paddle.color);

		/* render frame to screen */
		SDL_Flip(screen);

		/* keep a steady frame rate */
		Uint8 elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime < FRAME_DURATION)
			SDL_Delay(FRAME_DURATION - elapsedTime);
	}

	/* free resources */
	free(court);
	free(player1);
	free(player2);

	TTF_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}

inline void clearScreen(SDL_Surface *screen, Uint32 clearColor)
{
	SDL_FillRect(screen, &screen->clip_rect, clearColor);
}

Court *makeCourt(SDL_Surface *screen)
{
	/* the thickness of the court's walls */
	const Uint8 wt = 30;

	Court *court = malloc(sizeof(Court));
	court->x = 0;
	court->y = 0;
	court->w = C_WIDTH;
	court->h = C_HEIGHT;

	court->upperWall.x = 0;
	court->upperWall.y = 0;
	court->upperWall.w = C_WIDTH;
	court->upperWall.h = wt;

	court->lowerWall.x = 0;
	court->lowerWall.y = C_HEIGHT - wt;
	court->lowerWall.w = C_WIDTH;
	court->lowerWall.h = wt;

	court->color = SDL_MapRGB(screen->format, 0, 0, 0);

	SDL_FillRect(screen, &court->upperWall, court->color);
	SDL_FillRect(screen, &court->lowerWall, court->color);

	return court;
}

Player *makePlayer(SDL_Surface *screen)
{
	Player *player = malloc(sizeof(Player));

	/* make the leftmost player (x = 0) by default */
	player->paddle.rect.x = 0;

	player->paddle.rect.y = C_HEIGHT/2 - P_HEIGHT/2;
	player->paddle.rect.w = P_WIDTH;
	player->paddle.rect.h = P_HEIGHT;
	player->paddle.color = SDL_MapRGB(screen->format, 0, 0, 0);
	player->points = 0;

	return player;
}

void readPlayerInput(bool *running,
		bool *player1ShouldMoveUp, bool *player1ShouldMoveDown,
		bool *player2ShouldMoveUp, bool *player2ShouldMoveDown)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			*running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_q:
				*running = false;
				break;

			case P1_UP_KEY:
				*player1ShouldMoveUp = true;
				break;
			case P1_DOWN_KEY:
				*player1ShouldMoveDown = true;
				break;
			case P2_UP_KEY:
				*player2ShouldMoveUp = true;
				break;
			case P2_DOWN_KEY:
				*player2ShouldMoveDown = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case P1_UP_KEY:
				*player1ShouldMoveUp = false;
				break;
			case P1_DOWN_KEY:
				*player1ShouldMoveDown = false;
				break;
			case P2_UP_KEY:
				*player2ShouldMoveUp = false;
				break;
			case P2_DOWN_KEY:
				*player2ShouldMoveDown = false;
				break;
			}
			break;
		}

	}
}

void movePaddle(Court *court, Paddle *paddle, Direction direction)
{
	if (direction == UP) {
		Uint32 upperBound = court->y + court->upperWall.h;
		if (paddle->rect.y > upperBound+P_STEP)
			paddle->rect.y -= P_STEP;
		else
			paddle->rect.y = court->y + court->upperWall.h;
	} else if (direction == DOWN) {
		Uint32 lowerBound = court->y + court->h - court->lowerWall.h;
		if (paddle->rect.y+paddle->rect.h < lowerBound-P_STEP)
			paddle->rect.y += P_STEP;
		else
			paddle->rect.y = lowerBound - paddle->rect.h;
	}
}
