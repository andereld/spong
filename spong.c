#include "spong.h"

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	/* the frame buffer */
	SDL_Surface *screen = SDL_SetVideoMode(W_WIDTH, W_HEIGHT,
			W_COLOR_DEPTH, SDL_SWSURFACE);

	/* the background color of the screen */
	Uint32 clearColor = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	SDL_FillRect(screen, &screen->clip_rect, clearColor);

	/* set up the playing court */
	Uint8 wt = 30;	/* the thickness of the court's walls */
	Court court = {
		x: 0,
		y: 0,
		w: C_WIDTH,
		h: C_HEIGHT,
		upperWall: { x: 0, y: 0, w: C_WIDTH, h: wt },
		lowerWall: { x: 0, y: C_HEIGHT-wt, w: C_WIDTH, h: wt },
		color: SDL_MapRGB(screen->format, 0, 0, 0)
	};
	SDL_FillRect(screen, &court.upperWall, court.color);
	SDL_FillRect(screen, &court.lowerWall, court.color);

	/* set up the players and their paddles */
	Player player1 = {
		paddle: {
			rect: {
				x: 0,
				y: C_HEIGHT/2 - P_HEIGHT/2,
				w: P_WIDTH,
				h: P_HEIGHT
			},
			color: SDL_MapRGB(screen->format, 0, 0, 0)
		},
		points: 0
	};
	Player player2 = player1;
	player2.paddle.rect.x = C_WIDTH - P_WIDTH;

	/* set up the player controls; these need to be #defines to satisfy
	   the compiler's idea of an 'integer constant' */
#define P1_UP_KEY SDLK_a
#define P1_DOWN_KEY SDLK_z
#define P2_UP_KEY SDLK_i
#define P2_DOWN_KEY SDLK_PERIOD
	/* because SDL_KEY(UP|DOWN) only occur once, not continuously while
	   the key is pressed, we need to keep track of whether a key is
	   (still) pressed */
	bool player1ShouldMoveUp = false, player1ShouldMoveDown = false,
	     player2ShouldMoveUp = false, player2ShouldMoveDown = false;

	Uint32 startTime;	/* used to note the beginning of each
				   iteration of the main event loop */
	bool running = true;	/* true till the application should exit */
	while (running) {
		startTime = SDL_GetTicks();

		/* overwrite the previous frame's paddles */
		SDL_FillRect(screen, &player1.paddle.rect, clearColor);
		SDL_FillRect(screen, &player2.paddle.rect, clearColor);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
					running = false;
					break;

				case P1_UP_KEY:
					player1ShouldMoveUp = true;
					break;
				case P1_DOWN_KEY:
					player1ShouldMoveDown = true;
					break;
				case P2_UP_KEY:
					player2ShouldMoveUp = true;
					break;
				case P2_DOWN_KEY:
					player2ShouldMoveDown = true;
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case P1_UP_KEY:
					player1ShouldMoveUp = false;
					break;
				case P1_DOWN_KEY:
					player1ShouldMoveDown = false;
					break;
				case P2_UP_KEY:
					player2ShouldMoveUp = false;
					break;
				case P2_DOWN_KEY:
					player2ShouldMoveDown = false;
					break;
				}
				break;
			}

		}

		if (player1ShouldMoveUp)
			movePaddle(&court, &player1.paddle, UP);
		else if (player1ShouldMoveDown)
			movePaddle(&court, &player1.paddle, DOWN);
		if (player2ShouldMoveUp)
			movePaddle(&court, &player2.paddle, UP);
		else if (player2ShouldMoveDown)
			movePaddle(&court, &player2.paddle, DOWN);

		/* draw the paddles */
		SDL_FillRect(screen, &player1.paddle.rect,
				player1.paddle.color);
		SDL_FillRect(screen, &player2.paddle.rect,
				player2.paddle.color);

		/* render frame to screen */
		SDL_Flip(screen);

		/* keep a steady frame rate */
		Uint8 elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime < FRAME_DURATION)
			SDL_Delay(FRAME_DURATION - elapsedTime);
	}

	TTF_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
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
