#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

Mat frame;
VideoCapture vid;
char c;
int offset = 2;
bool pause = true;

const string default_name = "roi.txt";
fstream roi_File;
string roi_File_Name;
string line_File;

fstream setting_file;
const string settings_name = "settings.txt";

const string track_Bar_Name = "Setings";
int trackblur, track_thresh_cont, track_canny;

vector<Point2f> Points(4);
vector<vector<Point2f>> contoursPoints;
vector<int> contourIds;

vector<Mat> displayroi;
Mat display_final;
Mat ad = Mat::zeros(Size(150, 150), CV_8UC3); 
Mat bd = Mat::zeros(Size(150, 150), CV_8UC3); 
Mat cd = Mat::zeros(Size(150, 150), CV_8UC3); 
Mat dd = Mat::zeros(Size(150, 150), CV_8UC3);;
vector<Mat> aa;
vector<Mat> ba;
vector<Mat> ca;
vector<Mat> da;

Scalar colorBlack = Scalar(0, 0, 0);
Scalar colorBlue = Scalar(255, 0, 0);
Scalar colorGreen = Scalar(0, 255, 0);
Scalar colorRed = Scalar(0, 0, 255);
Scalar colorWhite = Scalar(255, 255, 255);

void get_roi();
void help();
void saveSettings();
Mat display_roi();
Mat display_roi_processed();
Mat display_roi_selection();
Mat blurFrame(Mat resized, int i, vector<Point2f>& points);
Mat processFrame(Mat resized, int i, vector<Point2f>& points);