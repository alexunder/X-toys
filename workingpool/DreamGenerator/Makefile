#  Makefile for glutMaster demo
#  version 0.3
#  George Stetten and Korin Crawford

FILES   = glutMaster.cpp main.cpp glutWindow.cpp demoWindow.cpp

OBJECTS = glutMaster.o main.o glutWindow.o demoWindow.o

EXECFIL = run
LIBS = -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
CC = g++

# Installation at UNC Computer Science
FLAGS = -L/afs/unc/proj/walk/lib/sgi_irix -I/afs/unc/proj/walk/include

# Installation at Duke University (George Stetten's lab)
#FLAGS =

$(EXECFIL): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(EXECFIL) $(LIBS)

.c.o :
	$(CC) $(FLAGS) -c $<

.c++.o :
	$(CC) $(FLAGS) -c $<
