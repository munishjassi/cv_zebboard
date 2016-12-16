LIB = ../Library/Optical_Flow 
CFHD = ../Library/config_file_handle
CXXFLAGS = -Wl,--no-undefined -I. -I$(LIB) -I$(CFHD) -L/usr/local/lib -I/usr/local/include/opencv -I/usr/local/include/opencv2
LIB = ../Library/Optical_Flow
LIBS = -lopencv_ml
OTHER_LIBS = -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc -lopencv_legacy -lopencv_video $(LIBS)

TARGETS = $(patsubst $(LIB)/%.cpp, $(LIB)/%.o, $(shell ls $(LIB)/*.cpp))

all: $(TARGETS) optical

%.o: %.cpp
	g++ $(CXXFLAGS) -o  $@ -c $< $(LIBS)

optical:  /home/linaro/complete_system/opticalFlow/src/main_Census_flower2.cpp
	g++ $(CXXFLAGS) $(TARGETS) $(CFHD)/config_file_handle.cpp main_Census_flower2.cpp -o optical -O0 -g $(OTHER_LIBS) -Wl,--no-undefined
	
