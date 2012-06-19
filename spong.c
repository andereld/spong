#include "spong.h"

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	/* the frame buffer */
	SDL_Surface *screen = SDL_SetVideoMode(W_WIDTH, W_HEIGHT, 32,
			SDL_SWSURFACE);

	/* the background color of the screen */
	Uint32 clearColor = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	SDL_FillRect(screen, &screen->clip_rect, clearColor);

	Uint32 startTime;	/* used to note the beginning of each
				   iteration of the main event loop */
	bool running = true;	/* true till the application should exit */
	while (running) {
		startTime = SDL_GetTicks();

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
				}
				break;
			}

		}

		/* render frame to screen */
		SDL_Flip(screen);

		/* keep a steady frame rate */
		if (SDL_GetTicks()-startTime < FRAME_DURATION)
			SDL_Delay(FRAME_DURATION - (SDL_GetTicks()-startTime));

	}

	TTF_Quit();

	SDL_Quit();

	return EXIT_SUCCESS;
}
