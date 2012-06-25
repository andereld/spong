#include "spong.h"

int main(int argc, char *argv[])
{
	/* initialize SDL and its subsystems */
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) die();
	if (TTF_Init() == -1) die();

	/* set the height and width of the main window, as well as the number
	   of bits per pixel; this needs to be done prior to initializing the 
	   main window (*screen, below) */
	initWindowAttributes();

	/* the frame buffer */
	SDL_Surface *screen = SDL_SetVideoMode(W_WIDTH, W_HEIGHT,
			W_COLOR_DEPTH, SDL_HWSURFACE|SDL_FULLSCREEN);
	if (!screen) die();

	/* hide the mouse cursor */
	SDL_ShowCursor(SDL_DISABLE);

	/* the background color of the screen */
	const Uint32 clearColor = CLEAR_COLOR(screen->format);
	clearScreen(screen, clearColor);

	/* clearColor as an SDL_Color, for use with TTF */
	Uint8 r, g, b;
	SDL_GetRGB(clearColor, screen->format, &r, &g, &b);
	SDL_Color bgColor = { r: r, g: g, b: b };

	/* the score font */
	TTF_Font *font = TTF_OpenFont("resources/VeraMono.ttf", FONT_SIZE);
	if (!font) die();
	SDL_Color fontColor = FONT_COLOR;

	/* the score text; we'll allow three digits plus the terminating '\0' */
	char *lScoreStr = malloc(sizeof(char) * 4);
	char *rScoreStr = malloc(sizeof(char) * 4);
	if (!lScoreStr || !rScoreStr) die();
	SDL_Surface *lScore = NULL, *rScore = NULL;
	SDL_Rect lScorePos = { x: C_X+FONT_SIZE, y: C_HEIGHT/5,
		w: F_WIDTH, h: F_HEIGHT };
	SDL_Rect rScorePos = { x: (C_X+C_WIDTH)-3*FONT_SIZE, y: C_HEIGHT/5,
		w: F_WIDTH, h: F_HEIGHT };

	/* set up the playing court */
	Court *court = makeCourt(screen);
	if (!court) die();

	/* set up the players and their paddles */
	Player *lPlayer = makePlayer(screen);
	Player *rPlayer = makePlayer(screen);
	if (!lPlayer || !rPlayer) die();
	rPlayer->paddle.rect.x = C_X + C_WIDTH - P_WIDTH;

	/* add the ball */
	Ball *ball = makeBall(screen);
	if (!ball) die();

	/* because SDL_KEY(UP|DOWN) occurs only once, not continuously while
	   the key is pressed, we need to keep track of whether a key is
	   (still) pressed */
	bool lPlayerShouldMoveUp = false, lPlayerShouldMoveDown = false,
	     rPlayerShouldMoveUp = false, rPlayerShouldMoveDown = false;

	Uint32 startTime;	/* denotes the beginning of each iteration
				   of the main event loop */
	bool running = true;	/* true till the application should exit */
	while (running) {
		startTime = SDL_GetTicks();

		/* clear the previous frame's paddles and ball */
		SDL_FillRect(screen, &lPlayer->paddle.rect, clearColor);
		SDL_FillRect(screen, &rPlayer->paddle.rect, clearColor);
		SDL_FillRect(screen, &ball->rect, clearColor);

		/* clear the previous frame's score */
		SDL_FillRect(screen, &lScorePos, clearColor);
		SDL_FillRect(screen, &rScorePos, clearColor);

		/* redraw the walls in case they were clipped by the ball
		   in a previous frame */
		SDL_FillRect(screen, &court->upperWall, court->color);
		SDL_FillRect(screen, &court->lowerWall, court->color);

		/* get the current state of the players' controls */
		readPlayerInput(&running,
				&lPlayerShouldMoveUp, &lPlayerShouldMoveDown,
				&rPlayerShouldMoveUp, &rPlayerShouldMoveDown);

		/* save the current position of the paddles */
		lPlayer->paddle.prevY = lPlayer->paddle.rect.y;
		rPlayer->paddle.prevY = rPlayer->paddle.rect.y;

		/* move the paddles if appropriate */
		if (lPlayerShouldMoveUp)
			movePaddle(court, &lPlayer->paddle, UP);
		else if (lPlayerShouldMoveDown)
			movePaddle(court, &lPlayer->paddle, DOWN);
		if (rPlayerShouldMoveUp)
			movePaddle(court, &rPlayer->paddle, UP);
		else if (rPlayerShouldMoveDown)
			movePaddle(court, &rPlayer->paddle, DOWN);

		/* move the ball */
		moveBall(court, ball, lPlayer, rPlayer);

		/* update the score */
		updateScore(ball, lPlayer, rPlayer);

		/* update the on-screen score */
		if (lScore) SDL_FreeSurface(lScore);
		snprintf(lScoreStr, 4, "%2d", lPlayer->points);
		lScore = TTF_RenderText_Shaded(font, lScoreStr, fontColor,
				bgColor);
		if (rScore) SDL_FreeSurface(rScore);
		snprintf(rScoreStr, 4, "%2d", rPlayer->points);
		rScore = TTF_RenderText_Shaded(font, rScoreStr, fontColor,
				bgColor);

		/* draw the score */
		SDL_BlitSurface(lScore, NULL, screen, &lScorePos);
		SDL_BlitSurface(rScore, NULL, screen, &rScorePos);

		/* draw the paddles */
		SDL_FillRect(screen, &lPlayer->paddle.rect,
				lPlayer->paddle.color);
		SDL_FillRect(screen, &rPlayer->paddle.rect,
				rPlayer->paddle.color);

		/* draw the ball */
		SDL_FillRect(screen, &ball->rect, ball->color);

		/* render frame to screen */
		SDL_Flip(screen);

		/* keep a steady frame rate */
		Uint8 elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime < FRAME_DURATION)
			SDL_Delay(FRAME_DURATION - elapsedTime);
	}

	/* free resources */
	free(lScoreStr);
	free(rScoreStr);
	free(court);
	free(lPlayer);
	free(rPlayer);
	free(ball);

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(lScore);
	SDL_FreeSurface(rScore);
	SDL_Quit();

	return EXIT_SUCCESS;
}

inline void clearScreen(SDL_Surface *screen, Uint32 clearColor)
{
	SDL_FillRect(screen, &screen->clip_rect, clearColor);
}

Court *makeCourt(SDL_Surface *screen)
{
	Court *court = malloc(sizeof(Court));

	court->x = C_X;
	court->y = C_Y;
	court->w = C_WIDTH;
	court->h = C_HEIGHT;

	court->upperWall.x = C_X;
	court->upperWall.y = C_Y;
	court->upperWall.w = C_WIDTH;
	court->upperWall.h = C_WALL;

	court->lowerWall.x = C_X;
	court->lowerWall.y = C_Y + C_HEIGHT - C_WALL;
	court->lowerWall.w = C_WIDTH;
	court->lowerWall.h = C_WALL;

	court->color = WALL_COLOR(screen->format);

	SDL_FillRect(screen, &court->upperWall, court->color);
	SDL_FillRect(screen, &court->lowerWall, court->color);

	return court;
}

Player *makePlayer(SDL_Surface *screen)
{
	Player *player = malloc(sizeof(Player));

	/* make the leftmost player (x = C_X) by default */
	player->paddle.rect.x = C_X;
	player->paddle.rect.y = (C_Y+C_HEIGHT)/2 - P_HEIGHT/2;
	player->paddle.prevY = player->paddle.rect.y;
	player->paddle.rect.w = P_WIDTH;
	player->paddle.rect.h = P_HEIGHT;
	player->paddle.color = PADDLE_COLOR(screen->format);
	player->points = 0;

	return player;
}

Ball *makeBall(SDL_Surface *screen)
{
	Ball *ball = malloc(sizeof(Ball));

	ball->rect.x = (C_X+C_WIDTH)/2 - B_SIZE/2;
	ball->rect.y = (C_Y+C_HEIGHT)/2 - B_SIZE/2;
	ball->rect.w = B_SIZE;
	ball->rect.h = B_SIZE;
	ball->color = BALL_COLOR(screen->format);

	ball->vx = B_VX;
	ball->vy = 0;

	return ball;
}

void readPlayerInput(bool *running,
		bool *lPlayerShouldMoveUp, bool *lPlayerShouldMoveDown,
		bool *rPlayerShouldMoveUp, bool *rPlayerShouldMoveDown)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			*running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case QUIT_KEY:
				*running = false;
				break;

			case LPLAYER_UP_KEY:
				*lPlayerShouldMoveUp = true;
				break;
			case LPLAYER_DOWN_KEY:
				*lPlayerShouldMoveDown = true;
				break;
			case RPLAYER_UP_KEY:
				*rPlayerShouldMoveUp = true;
				break;
			case RPLAYER_DOWN_KEY:
				*rPlayerShouldMoveDown = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case LPLAYER_UP_KEY:
				*lPlayerShouldMoveUp = false;
				break;
			case LPLAYER_DOWN_KEY:
				*lPlayerShouldMoveDown = false;
				break;
			case RPLAYER_UP_KEY:
				*rPlayerShouldMoveUp = false;
				break;
			case RPLAYER_DOWN_KEY:
				*rPlayerShouldMoveDown = false;
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

void moveBall(Court *court, Ball *ball, Player *lPlayer, Player *rPlayer)
{
	Paddle *lPaddle = &lPlayer->paddle;
	Paddle *rPaddle = &rPlayer->paddle;

	/* set the velocity of the ball */
	if (ball->rect.x <= lPaddle->rect.x+lPaddle->rect.w && ball->vx < 0) {
		/* the ball is on the left boundary of the court
		   and traveling leftwards */
		Uint32 upperBound = lPaddle->rect.y;
		Uint32 lowerBound = lPaddle->rect.y + lPaddle->rect.h;
		if (ball->rect.y+ball->rect.h >= upperBound
				&& ball->rect.y <= lowerBound) {
			ball->vx = -ball->vx;
			if (lPaddle->prevY != lPaddle->rect.y)
				ball->vy += lPaddle->prevY - lPaddle->rect.y;
			/* make sure the ball isn't clipped by the paddle */
			ball->rect.w = ball->rect.h = B_SIZE;
		}
	} else if (ball->rect.x+ball->rect.w >= rPaddle->rect.x
			&& ball->vx > 0) {
		/* the ball is on the right boundary of the court
		   and traveling rightwards */
		Uint32 upperBound = rPaddle->rect.y;
		Uint32 lowerBound = rPaddle->rect.y + rPaddle->rect.h;
		if (ball->rect.y+ball->rect.h >= upperBound
				&& ball->rect.y <= lowerBound) {
			ball->vx = -ball->vx;
			if (rPaddle->prevY != rPaddle->rect.y)
				ball->vy += rPaddle->prevY - rPaddle->rect.y;
			/* make sure the ball isn't clipped by the paddle */
			ball->rect.w = ball->rect.h = B_SIZE;
		}
	}
	if (ball->rect.y <= court->upperWall.y+court->upperWall.h
			|| ball->rect.y+ball->rect.h >= court->lowerWall.y) {
		/* the ball has collided with a wall */
		ball->vy = -ball->vy;
	}

	/* actually move the ball */
	ball->rect.x += ball->vx;
	ball->rect.y += ball->vy;
}

void updateScore(Ball *ball, Player *lPlayer, Player *rPlayer)
{
	if (ball->rect.x >= C_X+C_WIDTH) {
		/* lPlayer scored a point */
		lPlayer->points++;
		resetAndServe(ball, LEFT);
	} else if (ball->rect.x+ball->rect.w <= C_X) {
		/* rPlayer scored a point */
		rPlayer->points++;
		resetAndServe(ball, RIGHT);
	}
}

void resetAndServe(Ball *ball, Direction direction)
{
	/* the ball will be clipped by the edge of the screen,
	   so we have to set its width and height to the correct value */
	ball->rect.w = ball->rect.h = B_SIZE;

	/* center the ball on the court */
	ball->rect.x = (C_X+C_WIDTH)/2 - B_SIZE/2;
	ball->rect.y = (C_Y+C_HEIGHT)/2 - B_SIZE/2;

	/* the winner serves */
	ball->vy = 0;
	if (direction == RIGHT)
		ball->vx = -B_VX;
	else if (direction == LEFT)
		ball->vx = B_VX;
}

inline void die()
{
	if (strcmp(SDL_GetError(), "") != 0)
		fprintf(stderr, "spong: %s\n", SDL_GetError());
	if (errno)
		perror("spong: ");
	exit(1);
}
