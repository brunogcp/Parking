#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

VideoCapture vid;
Mat src, copySrc, srcTemp;

const string roi_File_Name = "roi_temp.txt";
fstream roi_File;

const string roi_File_Name_Out = "roi.txt";
fstream file;

const string nameWindow = "Place";

char c;
bool saved = 0;
bool deleted = 0;

int clickCount = 0;
int ids = 1;

vector<Vec2i> coordinates(4);
vector<Point2f> finalCoordinateId(1);
vector<Point> points;
Rect squareRect;

Scalar colorBlack = Scalar(0, 0, 0);
Scalar colorBlue = Scalar(255, 0, 0);
Scalar colorGreen = Scalar(0, 255, 0);
Scalar colorRed = Scalar(0, 0, 255);
Scalar colorWhite = Scalar(255, 255, 255);

void saveRoiOut();
void onMouseCallback(int event, int x, int y, int flags, void* userdata);
void handleClickProgress();
void handleDone();
void drawContoursFunction(Mat image, vector<vector<Vec2i>> coordinates, int label, Scalar color);
void save_roi(vector<Vec2i> cord, int id);
void help();