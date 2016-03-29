
CXX_SRC=main.cpp rast2d.cpp rast3d.cpp bucket.cpp sample.cpp parallel.cpp
CXX_OBJS=$(CXX_SRC:.cpp=.o)
HEADERS=mbrast.h timing.h

CXX=g++
CXXFLAGS=-O2 -Wall
ifeq ($(shell uname),Linux)
CXXFLAGS+= -DPBRT_IS_LINUX
CXXFLAGS+= -pthread
endif

mbrast: $(CXX_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(CXX_OBJS)

clean:
	/bin/rm -f $(CXX_OBJS) mbrast *~

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

# hack instead of having proper dependencies, but this will do
%.o: $(HEADERS)
