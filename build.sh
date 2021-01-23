gcc -c -g3 -Wall -Wextra -Werror -pedantic -lm -o bigint.o bigint.c
gcc -c -g3 -Wall -Wextra -Werror -pedantic -lm -o main.o main.c
gcc -lm -o main main.o bigint.o