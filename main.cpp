#include "main.h"

using namespace std;


JPG_dat *jpg;
Mat *qr_img;
vector<Mat> label_vec;

void Mat::draw(Point pt,int zoom, float r, float g, float b){
	glBegin(GL_POINTS);
	for(int j= 0;j<y;j++){
	for(int i= 0;i<x;i++){
		if( img[j*x+i] == 255)
			//glColor3d(1.0,1.0,1.0);
			continue;
		else
			//glColor3d(0.0,0.0,0.0);
			glColor3d(r,g,b);
		
		for(int px=0;px<zoom;px++){
		for(int py=0;py<zoom;py++){
			glVertex2i(pt.x + i*zoom+px, pt.y + (y-j)*zoom+py);
			}}
		//glVertex2i((i + pt.x)*4, ((y-j) + pt.y)*4);
	}}
	glEnd();
}



int JPG_dat::load(const char filename[]){
  unsigned long data_size;     // length of the file
  int channels;               //  3 =>RGB   4 =>RGBA 
  unsigned int type;  
  unsigned char * rowptr[1];    // pointer to an array
  struct jpeg_decompress_struct info; //for our jpeg info
  struct jpeg_error_mgr err;          //the error handler
  FILE* file = fopen(filename, "rb");  //open the file

  info.err = jpeg_std_error(& err);     
  jpeg_create_decompress(& info);   //fills info structure

  //if the jpeg file doesn't load
  if(!file) {
     fprintf(stderr, "Error reading JPEG file %s!", filename);
     return 0;
  }

  jpeg_stdio_src(&info, file);    
  jpeg_read_header(&info, TRUE);   // read jpeg file header

  jpeg_start_decompress(&info);    // decompress the file

  //set width and height
  width = info.output_width;
  height = info.output_height;
  channels = info.num_components;

  switch(channels){
  	case 4:
		type = GL_RGBA; break;
	case 3:
  		type = GL_RGB; break;
  	case 1:
  		type = GL_UNSIGNED_BYTE; break;
	}

	printf("channle is %d\n ", channels);

  arr = (unsigned char *)malloc( height * width * 3);
  printf("info output=width and height is %d %d\n", info.output_width, info.output_height);

  while (info.output_scanline < info.output_height) // loop
  {
    // Enable jpeg_read_scanlines() to fill our jdata array
    rowptr[0] = (unsigned char *)arr +  // secret to method
            3* info.output_width * info.output_scanline; 

    jpeg_read_scanlines(&info, rowptr, 1);
  }

  jpeg_finish_decompress(&info);   //finish decompressing


  jpeg_destroy_decompress(&info);
  fclose(file);                    //close the file

  return 0;
}


void Mat::white(){
	for(int i =0;i<x;i++){
	for(int j =0;j<y;j++){
		img[j*x+i] = 255;
	}}
}

Point Mat::center(){
	Point pt;
	pt.x = 0;
	pt.y = 0;
	int num_elem = 0;
	for(int i=0;i<x;i++){
	for(int j=0;j<y;j++){
		if(img[j*x+i] == 0){
			pt.x += i;
			pt.y += j;
			num_elem ++;
		}
	}}
	pt.x = pt.x / num_elem;
	pt.y = pt.y / num_elem;
	return pt;
}

int Mat::shape(){
	int center_x = 0;
	int center_y = 0;
	int num_elem = 0;
	for(int i=0;i<x;i++){
	for(int j=0;j<y;j++){
		if(img[j*x+i] == 0){
			center_x += i;
			center_y += j;
			num_elem ++;
		}
	}}
	center_x = center_x / num_elem;
	center_y = center_y / num_elem;

	vector<float> dist_list;	
	for(int i=0;i<x;i++){
	for(int j=0;j<y;j++){
		if(img[j*x+i] == 0){
			dist_list.push_back ( sqrt( pow(center_x -i,2)+ pow(center_y -j ,2)) );
		}
	}}
	if(num_elem == 0)
		return 0;

	float mean_dist=0;
	for(int i=0;i<dist_list.size();i++){
		mean_dist += dist_list[i];	
	}
	mean_dist = mean_dist/dist_list.size();
	
	float sigma= 0;
	for(int i=0;i<dist_list.size();i++){
		sigma += pow( mean_dist - dist_list[i], 2);
	}
	sigma = sqrt(sigma/(float)num_elem);

	//printf("sigma/mean_dist is %f\n", sigma/mean_dist);
	if(mean_dist == 0)
		return 0;
	if(sigma/mean_dist < 0.25)
		return 1;
	else
		return 0;
}


void JPG_dat::rgb2bin( Mat* dst){
	
	dst->x = width; 
	dst->y = height;
	dst->img = (int*)malloc(sizeof(int)*dst->x*dst->y);
	for(int j= 0;j<dst->y;j++){
	for(int i= 0;i<dst->x;i++){
		if( arr[((j*3)*width + i)] > 128){
			dst->img[j*width+ i] = 255;
		}else{
			dst->img[j*width+ i] = 0;
		}
	}}
}

void JPG_dat::draw( int x_off, int y_off ){

	glBegin(GL_POINTS);
	for(int j= 0;j<height;j++){
	for(int i= 0;i<width;i++){
		float v = (float)arr[((j*3)*(width) + i) ]/256;
		glColor3d(v,v,v);
		//glColor3f(r,g,b);
		glVertex2i(i + x_off, (height-j) + y_off);
	}}
	glEnd();
	
	//printf("num of non zero pixel is %d\n", stat);
}

int Mat::diam()
{
	int max_x=0;
	int min_x=x;
	int max_y=0;
	int min_y=y;

	for(int i =0;i<x;i++){
	for(int j =0;j<y;j++){
		if(img[j*x+i] == 0){
			if(max_x < i)
				max_x = i;
			if(min_x > i)
				min_x = i;
			if(max_y < j)
				max_y = j;
			if(min_y > j)
				min_y = j;
		}
	}}
	//printf("%d %d %d %d\n", max_x, min_x, max_y, min_y);
	return max_x-min_x;
}


void display(void) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//jpg->draw( 0, 0);
	Point pt = Point(0, 0);
	qr_img->draw( pt, 1, 0.0,0.0,0.0);
	Point pt2 = Point(750,0);
	//label[0].draw(pt2,3);

	Mat *tmp = new Mat(qr_img->x, qr_img->y);
	Mat *src = new Mat(qr_img->x, qr_img->y);
	//*src = *qr_img;
	for(int i =0;i<src->x;i++){
	for(int j =0;j<src->y;j++){
		src->img[j*src->x+i] = qr_img->img[j*src->x + i];
	}}


	vector<Point> center;
	float pixel_length;
	while(1){
		tmp->white();

	if(_partial_labeling( src, tmp ) == 0)
		break;
	if( tmp->shape() == 1){
		pixel_length = (float)tmp->diam()/7;
		center.push_back(tmp->center());
		tmp->draw(pt2, 1, 1.0,0.0,0.0);
	}else{
		//tmp->white();
		pt =  tmp->center();
		tmp->draw(pt2, 1, 0.0,0.0,0.0);
		for(int i =0;i<center.size();i++){
			if((abs(pt.x - center[i].x) < 10) && ( abs(pt.y - center[i].y) < 10))
				tmp->draw(pt2, 1, 0.0,1.0,0.0);
		}
		}
	}
	int qr_len = abs(center[0].x - center[1].x)/pixel_length + 7;
	printf("size of pixel is %d\n", qr_len);
	glutSwapBuffers();
	glFlush();

}

void keyboard( unsigned char key, int x, int y){
	switch(key){
		case 'q':	
			exit(0);
		case 'r':
			break;
	}
}
void keyboard_up( unsigned char key, int x, int y){
	switch(key){
		case 'q':	
			exit(0);
		case 'r':{
			break;
		}
	}
}


// find a continuous pixel and returns its label and the cut image
void _labeling(Mat*src, Mat*dst, int px, int py) {
const int black =0;
const int white = 255;

	if( src->img[py*src->x + px] != black) 
		return ;
	else{
		src->img[py*src->x + px] = white;
		dst->img[py*src->x + px] = black;
	}

	if(px < src->x - 1)
		_labeling(src, dst, px+1, py);
	if(px > 0)
		_labeling(src, dst, px-1, py);
	if(py < src->y - 1)
		_labeling(src, dst, px, py+1);
	if(py > 0 )
		_labeling(src, dst, px, py-1);

	//src->img[off.y*src->x+off.x]

}

int _partial_labeling(Mat* src, Mat *dst){
	// find a point where we start
	int i=0 , j=0;
	while( (i< src->x) && (j < src->y)){
		if(src->img[j*src->x+i] == 0) // find black
			break;
		if(i == src->x-1){
			j++;
			i=0;
		}else
			i++;
	}
	if((i==src->x) || (j==src->y))
		return 0;

	_labeling( src, dst, i, j );
	return 1; // there still is a
}

void labeling(Mat* src, vector<Mat> *dst){

	//while(1){
	for(int i =0;i<20;i++){
		//Mat tmp = Mat(src->x, src->y);
		Mat tmp(src->x, src->y);
		tmp.white();
		_partial_labeling( src, &tmp);
		dst->push_back(tmp);
	}
}


void idle()
{
	glutPostRedisplay();
}

void init() {
	glClearColor(1.0,1.0,1.0,0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, WIDTH_SIZE, 0.0, HEIGHT_SIZE);
}


void * gl_loop_thread( void *threadid){
	glutMainLoop();
	return NULL;
}

void * time_watch_thread( void *threadid){

	static int count = 0;
	while( 1 ){
		count++;
		usleep(1000);
	}
	return NULL;
}



int main(int argc, char *argv[])
{

	srand( (unsigned int) time(&t));
	
	glutInitWindowPosition(400,0);
	glutInitWindowSize( WIDTH_SIZE, HEIGHT_SIZE);

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutIdleFunc(idle);

	init();
	
	if(argc == 1){
		printf("error: no file name\n");
		exit(0);
	}else
		jpg = new JPG_dat(argv[1]);

	qr_img = new Mat(jpg->width, jpg->height);
	jpg->rgb2bin(qr_img);
	//labeling(qr_img, &label_vec);


	pthread_t threads[2];
	int rc;
	rc = pthread_create(&threads[0], NULL, gl_loop_thread, (void*)0);
	rc = pthread_create(&threads[1], NULL, time_watch_thread, (void*)1);
	if(rc){
		printf("error\n");
		}
	
	pthread_exit(NULL);
return 0;
}







