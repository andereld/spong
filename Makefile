CC = gcc
CFLAGS = -lSDL -lSDL_mixer -lSDL_ttf

.DEFAULT: spong

spong: spong.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f spong
