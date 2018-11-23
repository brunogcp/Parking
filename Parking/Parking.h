#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include "timer.h"

using namespace std;
using namespace cv;

Mat frame, frame_gray, blur_Frame, thresh_Contrast, thresh_Canny, morph_Frame, frame_copy, frame_display;
int blur_Val, thresh_Contrast_Val, thresh_Canny_Val, detect_time_val;

fstream roi_File;
string roi_File_Name;
string roi_line_File;

fstream setting_File;
string settings_File_Name;
string settings_line_File;

fstream out_file;
string out_File_Name = "out.txt";

VideoCapture vid;
char c;
bool pause = true;
bool process_place_bool = false;
bool display = false;
bool display_process = false;
int offset = 2;

bool parking_Status[16] = { false , false, false , false , false , false , false , false , false , false , false , false , false , false , false , false };
bool parking_Status_temp[16] = { false , false, false , false , false , false , false , false , false , false , false , false , false , false , false , false };
int temp_timer = 0;
int temp_v[16];
int temp_f[16];

int syst_time_coutdown = 0;

vector<Point2f> Points_Place(4);
vector<vector<Point2f>> contoursPoints_Place;
vector<int> contourIds_Place;

vector<Mat> displayroi;
Mat display_final;
Mat ad, bd, cd, dd;
vector<Mat> aa;
vector<Mat> ba;
vector<Mat> ca;
vector<Mat> da;

Scalar colorBlack = Scalar(0, 0, 0);
Scalar colorBlue = Scalar(255, 0, 0);
Scalar colorGreen = Scalar(0, 255, 0);
Scalar colorRed = Scalar(0, 0, 255);
Scalar colorWhite = Scalar(255, 255, 255);

timer parkir_time[16];
timer system_timer;
timer timer_save;
timer detect_timer;
timer ini_detect_timer;

Mat display_roi();
Mat display_roi_processed();
Mat blurFrame(Mat resized, int i, vector<Point2f>& points);
Mat processFrame(Mat resized, int i, vector<Point2f>& points);

void get_roi_and_settings();
void help();
void process_place();
void save_Out_On();
void save_Out_Off();