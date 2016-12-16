CXXFLAGS = -Wl,--no-undefined -I. -L/usr/local/lib -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/lib 

LIBS = -lm

TARGETS = $(patsubst %.cpp, %.o, $(shell ls *.cpp))

all: $(TARGETS)

%.o: %.cpp
	g++ $(CXXFLAGS) -o  $@ -c $< $(LIBS)
	
