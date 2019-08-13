OBJS = cell.o maze.o stack.o queue.o da.o cda.o amaze.o
OOPTS = -Wall -Wextra -std=c99 -g -c
LOPTS = -Wall -Wextra -std=c99 -g

amaze : cell.o maze.o stack.o queue.o da.o cda.o amaze.o
	gcc $(LOPTS) cell.o maze.o stack.o queue.o da.o cda.o amaze.o -o amaze

amaze.o : amaze.c
	gcc $(OOPTS) amaze.c

cell.o : cell.c cell.h
	gcc $(OOPTS) cell.c

maze.o : maze.c maze.h
	gcc $(OOPTS) maze.c

stack.o : stack.c stack.h
	gcc $(OOPTS) stack.c

queue.o : queue.c queue.h
	gcc $(OOPTS) queue.c

da.o : da.c da.h
	gcc $(OOPTS) da.c

cda.o : cda.c cda.h
	gcc $(OOPTS) cda.c

test : amaze
	./amaze -c 3 3 m.data -s m.data m.solved -d m.solved
	./amaze -v

valgrind : amaze
	valgrind ./amaze -c 3 3 m.data -s m.data m.solved -d m.solved
	valgrind ./amaze -v

clean :
	rm -f $(OBJS) amaze
