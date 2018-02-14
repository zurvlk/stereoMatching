#Makefile

# CXX = icc
# CXXFLAGS = -Wall -parallel -lm -fPIC

CC=gcc

TERGET = grsa

all:graph.o bmp.o ford_fulkerson.o queue_stack.o grsa_drv.o grsa.o -lm 
	$(CC) graph.o bmp.o ford_fulkerson.o queue_stack.o grsa_drv.o grsa.o -o $(TERGET) -lm -g3 -std=c11
.c.o:
	$(CC) -c $< -Wall -l -g3 -std=c11

remove:
	rm -f *.o
clean:
	rm -f *.o *~ grsa
