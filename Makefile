CC=gcc

C_FLAGS=-Wall -Wextra -Werror -pedantic -Iincludes/
LD_FLAGS=-lm
DBG_FLAGS=-g3

main: main.o libbi.so
	$(CC) -o $@ $< -L. -lm -lbi -Wl,-rpath,.

main.o: main.c
	$(CC) -o $@ -c $< $(C_FLAGS)

libbi.so: bi_mem.o bi_display.o bi_ops.o bi_bits.o
	$(CC) -shared -o $@ $^ $(LD_FLAGS)

bi_mem.o: src/bi_mem.c
	$(CC) -o $@ -c -fPIC $< $(C_FLAGS) $(DBG_FLAGS)

bi_display.o: src/bi_display.c
	$(CC) -o $@ -c -fPIC $< $(C_FLAGS) $(DBG_FLAGS)

bi_ops.o: src/bi_ops.c
	$(CC) -o $@ -c -fPIC $< $(C_FLAGS) $(DBG_FLAGS)

bi_bits.o: src/bi_bits.c
	$(CC) -o $@ -c -fPIC $< $(C_FLAGS) $(DBG_FLAGS)