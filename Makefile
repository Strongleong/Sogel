game: game.c
	clang -Wall -Wextra -o game game.c -fsanitize=address -ggdb
