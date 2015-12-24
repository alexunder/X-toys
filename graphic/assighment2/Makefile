# Simple Makefile for 6.837 raycasting assignment
# for g++ on linux platforms

# ===============================================================

SRCS 	 	= main.cpp \
		  image.cpp \
		  matrix.cpp \
          scene_parser.cpp \
		  Sphere.cpp \
		  Plane.cpp \
		  Triangle.cpp \
		  OrthographicCamera.cpp \
		  PerspectiveCamera.cpp \
		  Group.cpp \
          Transform.cpp

# ===============================================================

LIBS            = -lm -L/usr/X11R6/lib
CC		= g++
EXE	  	= raytracer
OBJS =  $(SRCS:.C=.o)
CFLAGS = -std=c++0x
#-DDEBUG
# ===============================================================
# targets

.PHONY: all depend clean

all: depend $(EXE)

depend:
	$(CC) -E -M $(SRCS) > Makefile.depend

clean: 
	-rm -f *~ *bak *.o  core.* Makefile.depend $(EXE) 

# ===============================================================
# compilation rules

$(EXE): $(OBJS)
	$(CC) $(CFLAGS)  -o $@ $(OBJS) $(LIBS)

.C.o:
	$(CC) $< -c -o $@

.cpp.o:
	$(CC) $< -c -o $@

# ===============================================================

-include Makefile.depend

