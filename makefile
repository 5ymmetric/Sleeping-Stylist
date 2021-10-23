CC = gcc
FLAGS = -lpthread
INCLUDES = -lm

sleepingStylistSem: sleepingStylistSem.c
			$(CC) $(FLAGS) sleepingStylistSem.c -o sleepingStylistSem

clean:
			rm -f *~ *.o
