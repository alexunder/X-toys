# Simple Makefile for 6.837 raycasting assignment
# for g++ on linux platforms

# ===============================================================

SRCS 	 	= main.cpp \
		  image.cpp \
		  matrix.cpp \
          scene_parser.cpp \
		  Sphere.cpp \
		  OrthographicCamera.cpp \
		  Group.cpp

# ===============================================================

LIBS            = -lm -L/usr/X11R6/lib
CC		= g++
EXE	  	= raytracer
OBJS =  $(SRCS:.C=.o)

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
	$(CC) -o $@ $(OBJS) $(LIBS)

.C.o:
	$(CC) $< -c -o $@

.cpp.o:
	$(CC) $< -c -o $@

# ===============================================================

-include Makefile.depend

