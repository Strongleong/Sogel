tetris: tetris.c
	clang -Wall -Wextra -o tetris tetris.c -fsanitize=address -ggdb
