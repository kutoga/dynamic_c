main: dynamic_c.c main.c
	gcc -o $@ $^ -O3 -ldl -Wall -Wextra -Werror

