#include "Settings.h"

int main(int argc, char** argv) {
	if (argc != 4)
	{
		cout << "Usage: Settings.exe <Video_Filename or Camera_Number> <ParkingData_Filename> <Video or Webcam>" << endl << endl;
		cout << "<Camera_Number> can be 0, 1, ... for attached cameras" << endl;
		cout << "<ParkingData_Filename> should be simple txt file with lines of : id x1 y1 x2 y2 x3 y3 x4 y4" << endl;
		cout << "<Video or Webcam> choose between video or Webcam" << endl;		
		system("PAUSE");
		return -1;
	}
	
	roi_File_Name = string(argv[2]);
	get_roi();

	if (contoursPoints.size() <= 0)
	{
		cout << "Error Reading Data File !" << endl;
		system("PAUSE");
		return -2;
	}


	if (string(argv[3]) == "Video" || string(argv[3]) == "Vid" || string(argv[3]) == "video" || string(argv[3]) == "vid" || string(argv[3]) == "V" || string(argv[3]) == "v")
	{
		vid.open(argv[1]);
	}
	else if (string(argv[3]) == "Webcam" || string(argv[3]) == "Web" || string(argv[3]) == "webcam" || string(argv[3]) == "web" || string(argv[3]) == "W" || string(argv[3]) == "w")
	{
		string String_temp = argv[1];
		int Int_temp = stoi(String_temp);
		vid.open(Int_temp);
	}
	else
	{
		cout << "<Video or Webcam> choose between video or picture or Webcam" << endl;
		system("PAUSE");
		return -3;
	}

	if (!vid.isOpened())
	{
		cout << "Error Opening video !" << endl;
		system("PAUSE");
		return -4;
	}

	try
	{
		int blurCte = 3;
		int thres_cont = 150;
		int canny_thresh = 25;

		namedWindow(track_Bar_Name, WINDOW_GUI_NORMAL);
		resizeWindow(track_Bar_Name, 300, 50);
		createTrackbar("blur", track_Bar_Name, &blurCte, 9);
		createTrackbar("Thresh cont", track_Bar_Name, &thres_cont, 255);
		createTrackbar("Canny thresh", track_Bar_Name, &canny_thresh, 300);

		namedWindow("Croped", WINDOW_GUI_NORMAL);
		namedWindow("Processed Crop", WINDOW_GUI_NORMAL);
		namedWindow("Selection Crop", WINDOW_GUI_NORMAL);
	}
	catch (const std::exception&)
	{
		cout << "Error Trackbar !" << endl;
		system("PAUSE");
		return -50;
	}

	try
	{
		cout << endl << "Press 'h' for help" << endl;
		while (true)
		{
			if (pause)
			{
				vid.read(frame);
			}

			if (frame.empty())
			{
				cout << "Erro reading frame !" << endl;
				system("PAUSE");
				return -5;
			}

			imshow("Croped", display_roi());
			imshow("Processed Crop", display_roi_processed());
			imshow("Selection Crop", display_roi_selection());

			c = (char)waitKey(30);
			if (c == 27) {
				break;
			}
			switch (c)
			{
			case ' ':
				pause = !pause;
				break;
			case 's':
				saveSettings();
				break;
			case 'h':
				help();
				break;
			}

			if (!vid.grab())
				break;
		}
	}
	catch (const std::exception&)
	{
		cout << "Error !" << endl;
		system("PAUSE");
		return -100;
	}

	vid.release();
	destroyAllWindows();
	system("PAUSE");
	return 0;
}

void help()
{
	cout << endl << "s = save settings in .txt file" << endl;
	cout << "space = play or pause video" << endl;
	cout << "Esc = exit aplication" << endl;
}

void saveSettings()
{
	setting_file.open(settings_name, ifstream::out | ifstream::trunc);
	if (!setting_file.is_open() || setting_file.fail())
	{
		setting_file.close();
		cout << "Error : failed to erase file content !" << endl;
	}
	setting_file.close();

	int blur_poss = getTrackbarPos("blur", track_Bar_Name);
	int thresh_cont_poss = getTrackbarPos("Thresh cont", track_Bar_Name);
	int thres_canny_poss = getTrackbarPos("Canny thresh", track_Bar_Name);

	setting_file.open(settings_name);

	if (!setting_file.is_open() || setting_file.fail())
	{
		setting_file.close();
		cout << "Error : failed to open file content !" << endl;
	}

	setting_file << blur_poss << endl;
	setting_file << thresh_cont_poss << endl;
	setting_file << thres_canny_poss << endl;
	setting_file << "5" << endl;

	setting_file.close();
	cout << "File saved" << endl;
}

void get_roi() {
	roi_File.open(roi_File_Name);

	if (!roi_File.is_open() || roi_File.fail())
	{
		roi_File.close();
		cout << "Error : failed to open file content !" << endl;
	}

	while (getline(roi_File, line_File))
	{
		istringstream iss(line_File);
		float x1, y1, x2, y2, x3, y3, x4, y4;
		int id;

		iss >> id >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;

		Points[0] = Point2f(x1, y1);
		Points[1] = Point2f(x2, y2);
		Points[2] = Point2f(x3, y3);
		Points[3] = Point2f(x4, y4);

		contoursPoints.push_back(Points);

		contourIds.push_back(id);

		cout << "Geting rois... [" << id << "]." << endl;
	}

	roi_File.close();
}

Mat display_roi()
{
	int i = 0;
	offset = 10;
	Mat emptyMat = Mat::zeros(Size(150, 150), CV_8UC3);

	for (vector<Point2f>& points : contoursPoints)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);

		Mat crop = frame(cropRect);
		Mat resized(Size(150, 150), crop.type());
		resize(crop, resized, Size(150, 150));

		displayroi.push_back(resized);
		if (i < 4)
		{
			aa.push_back(resized);
			hconcat(aa, ad);
		}
		else if (i < 8)
		{
			ba.push_back(resized);
			hconcat(ba, bd);
		}
		else if (i < 12)
		{
			ca.push_back(resized);
			hconcat(ca, cd);
		}
		else if (i < 16)
		{
			da.push_back(resized);
			hconcat(da, dd);
		}
		i++;
	}

	if (ba.size() == 3)
	{
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 2)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 1)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}

	if (ca.size() == 3)
	{
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 2)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 1)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}

	if (da.size() == 3)
	{
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 2)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 1)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}

	Mat FinalTemp;

	if (displayroi.size() <= 4)
	{
		ad.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 8)
	{
		vconcat(ad, bd, bd);
		bd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 12)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		cd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 16)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		vconcat(cd, dd, dd);
		dd.copyTo(FinalTemp);
	}

	displayroi.clear();
	aa.clear();
	ba.clear();
	ca.clear();
	da.clear();

	return FinalTemp;
}

Mat display_roi_processed()
{
	int i = 0;
	offset = 10;
	Mat emptyMat = Mat::zeros(Size(150, 150), CV_8UC1);

	for (vector<Point2f>& points : contoursPoints)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);

		Mat crop = frame(cropRect);
		Mat resized(Size(150, 150), crop.type());
		resize(crop, resized, Size(150, 150));

		resized = blurFrame(resized, i, points);

		displayroi.push_back(resized);
		if (i < 4)
		{
			aa.push_back(resized);
			hconcat(aa, ad);
		}
		else if (i < 8)
		{
			ba.push_back(resized);
			hconcat(ba, bd);
		}
		else if (i < 12)
		{
			ca.push_back(resized);
			hconcat(ca, cd);
		}
		else if (i < 16)
		{
			da.push_back(resized);
			hconcat(da, dd);
		}
		i++;
	}

	if (ba.size() == 3)
	{
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 2)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 1)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}

	if (ca.size() == 3)
	{
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 2)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 1)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}

	if (da.size() == 3)
	{
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 2)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 1)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}

	Mat FinalTemp;

	if (displayroi.size() <= 4)
	{
		ad.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 8)
	{
		vconcat(ad, bd, bd);
		bd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 12)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		cd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 16)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		vconcat(cd, dd, dd);
		dd.copyTo(FinalTemp);
	}

	displayroi.clear();
	aa.clear();
	ba.clear();
	ca.clear();
	da.clear();

	return FinalTemp;
}

Mat display_roi_selection()
{
	int i = 0;
	offset = 10;
	Mat emptyMat = Mat::zeros(Size(150, 150), CV_8UC1);

	for (vector<Point2f>& points : contoursPoints)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);

		Mat crop = frame(cropRect);
		Mat resized(Size(150, 150), crop.type());
		resize(crop, resized, Size(150, 150));

		resized = blurFrame(resized, i, points);
		resized = processFrame(resized, i, points);

		displayroi.push_back(resized);
		if (i < 4)
		{
			aa.push_back(resized);
			hconcat(aa, ad);
		}
		else if (i < 8)
		{
			ba.push_back(resized);
			hconcat(ba, bd);
		}
		else if (i < 12)
		{
			ca.push_back(resized);
			hconcat(ca, cd);
		}
		else if (i < 16)
		{
			da.push_back(resized);
			hconcat(da, dd);
		}
		i++;
	}

	if (ba.size() == 3)
	{
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 2)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}
	else if (ba.size() == 1)
	{
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		ba.push_back(emptyMat);
		hconcat(ba, bd);
	}

	if (ca.size() == 3)
	{
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 2)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}
	else if (ca.size() == 1)
	{
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		ca.push_back(emptyMat);
		hconcat(ca, cd);
	}

	if (da.size() == 3)
	{
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 2)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}
	else if (da.size() == 1)
	{
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		da.push_back(emptyMat);
		hconcat(da, dd);
	}

	Mat FinalTemp;

	if (displayroi.size() <= 4)
	{
		ad.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 8)
	{
		vconcat(ad, bd, bd);
		bd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 12)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		cd.copyTo(FinalTemp);
	}
	else if (displayroi.size() <= 16)
	{
		vconcat(ad, bd, bd);
		vconcat(bd, cd, cd);
		vconcat(cd, dd, dd);
		dd.copyTo(FinalTemp);
	}

	displayroi.clear();
	aa.clear();
	ba.clear();
	ca.clear();
	da.clear();

	return FinalTemp;
}

Mat blurFrame(Mat resized, int i, vector<Point2f>& points)
{
	Mat gray, blur, thresh;
	cvtColor(resized, gray, COLOR_BGR2GRAY);
	trackblur = getTrackbarPos("blur", track_Bar_Name);
	if (trackblur % 2 != 1)
	{
		trackblur--;
	}
	GaussianBlur(gray, blur, Size(trackblur, trackblur), 0, 0);
	track_thresh_cont = getTrackbarPos("Thresh cont", track_Bar_Name);
	threshold(blur, thresh, track_thresh_cont, 200, THRESH_TRUNC);
	return thresh;
}

Mat processFrame(Mat resized, int i, vector<Point2f>& points)
{
	Mat thresh, morph, canny;
	Mat contoursMatrix = Mat::zeros(resized.size(), resized.type());

	morphologyEx(resized, morph, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(2, 2)), Point(-1, -1), 1);

	track_canny = getTrackbarPos("Canny thresh", track_Bar_Name);
	Canny(morph, canny, track_canny, track_canny * 3, 3);

	vector<vector<Point> > contours;
	findContours(canny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > areaContours;
	for (vector<Point> &c : contours)
	{
		if (contourArea(c) > 4.5)
		{
			areaContours.push_back(c);
		}
	}
	contours.clear();

	drawContours(contoursMatrix, areaContours, -1, colorWhite, -1);

	string text = "Contours: ";
	text.append(to_string(areaContours.size()));
	putText(contoursMatrix, text, Point(15, 130), FONT_HERSHEY_COMPLEX_SMALL, 0.75, colorWhite, 1, 8);

	areaContours.clear();

	return contoursMatrix;
}