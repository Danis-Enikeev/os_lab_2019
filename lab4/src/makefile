CC=gcc
CFLAGS=-I.

all : parallel_min_max zombie process_memory parallel_sum

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

parallel_sum : utils.o sum.o utils.h sum.h 
	$(CC) -o parallel_sum utils.o sum.o parallel_sum.c -lpthread $(CFLAGS)

zombie : 
	$(CC) -o zombie zombie.c $(CFLAGS)    
	
process_memory : 
	$(CC) -o process_memory process_memory.c $(CFLAGS)    

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

sum.o : sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)


clean :
	rm sequential_fork utils.o find_min_max.o sequential_min_max parallel_min_max zombie process_memory sum.o parallel_sum
