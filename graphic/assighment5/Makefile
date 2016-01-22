# Simple Makefile for 6.837 raycasting assignment
# for g++ on linux platforms

# ===============================================================
# update this list as you add new C++ source files

SRCS 	 	= main.cpp \
		  matrix.cpp \
		  image.cpp \
		  scene_parser.cpp \
		  Sphere.cpp \
		  Triangle.cpp \
		  Plane.cpp \
	 	  Transform.cpp \
		  OrthographicCamera.cpp \
		  PerspectiveCamera.cpp \
		  Group.cpp \
		  light.cpp \
		  PhongMaterial.cpp \
          rayTree.cpp \
		  RayTracer.cpp \
          glCanvas.cpp
          BoundingBox.cpp
 #         material.cpp \
		  raytracer.C \
		  camera.cpp \

# ===============================================================
# NOTE:  On athena, the Mesa GL libraries are currently located in the
#        mesa_v501 locker.  Type "add mesa_v501" to access these files.

LIBS            = -lm -lGL -lGLU -lglut -L/usr/lib/x86_64-linux-gnu -lXmu -lXi
		 # -Xlinker -rpath -Xlinker /mit/mesa_v501/lib/ -L/mit/mesa_v501/lib/
INCLUDE 	= -I/usr/include/
CC		= g++
EXE	  	= raytracer
OBJS =  $(SRCS:.C=.o)
CFLAGS = -std=c++0x -DDEBUG

# ===============================================================
# targets

.PHONY: all depend clean

all: depend $(EXE)

depend:
	$(CC) $(INCLUDE) -E -M $(SRCS) > Makefile.depend

clean: 
	-rm -f *~ *bak *.o  core.* Makefile.depend $(EXE) 

# ===============================================================
# compilation rules

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(OBJS) $(LIBS)

.C.o:
	$(CC) $(INCLUDE) $< -c -o $@

# ===============================================================

-include Makefile.depend

