CC = gcc
CFLAGS = `sdl-config --cflags --libs` -lSDL_mixer -lSDL_ttf

.DEFAULT: spong

spong: spong.c spong.h
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f spong
