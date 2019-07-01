#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <cstring>

//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>

#include <pthread.h>

#include <jpeglib.h>    
#include <jerror.h>

#include <vector>
#include <iostream>


#include "font.h"


using namespace std;


#define WIDTH_SIZE 1500
#define HEIGHT_SIZE 1000

#define ID_ALLY 1
#define ID_ENEMY 2

time_t t;

class Point {
public:
	int x ;
	int y;
	Point(int src_x , int src_y){
		x = src_x;	
		y = src_y;
	}
	Point(){
	}
};

class Mat{
public:
	int x ;
	int y;
	int *img;
	Mat(int src_x , int src_y){
		x = src_x;	
		y = src_y;
		img  = (int*)malloc(sizeof(int)*src_x * src_y);
	}
	Mat(){
		//img  = (int*)malloc(sizeof(int)*src_x * src_y);
	}
	~Mat(){
		free(img);
	}

	void draw(Point pt, int zoom,float r, float g, float b);

	void white();
	int shape();
	Point center();

	int diam();
	
};


class JPG_dat{
public:
	int width, height;
	unsigned char *arr;
	

	int load(const char filename[]);
	void draw(int x_off,int y_off);
	void rgb2bin(Mat* dst);

	JPG_dat(const char filename[]){
		load(filename);
	}
	JPG_dat(){
	}
	~JPG_dat(){
		free (arr);
	}
	
};

int _partial_labeling(Mat* src, Mat *dst);




void display(void);
void keyboard( unsigned char key, int x, int y);
void keyboard_up( unsigned char key, int x, int y);


// find a continuous pixel and returns its label and the cut image
void _labeling(Mat*src, Mat*dst, int px, int py);

int _partial_labeling(Mat* src, Mat *dst);

void labeling(Mat* src, vector<Mat> *dst);

void idle();
void init();

void * gl_loop_thread( void *threadid);

void * time_watch_thread( void *threadid);
