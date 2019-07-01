
CFLAG=-I/usr/include/opencv4/
LFLAG=-lglut -lGLU -lGL -lc -lm -lpthread  -ljpeg

all : main.cpp
	g++ -o  run main.cpp $(LFLAG) $(CFLAG)

debug : 
	./run
	
