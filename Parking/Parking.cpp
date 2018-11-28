#include "Parking.h"
#include "Parking_functions.h"

int main(int argc, char** argv) {
	if (argc != 5)
	{
		cout << "Usage: Settings.exe <Video_Filename or Camera_Number> <ParkingData_Filename> <ParkingSettings_Filename> <Video or Webcam>" << endl << endl;
		cout << "<Camera_Number> can be 0, 1, ... for attached cameras" << endl;
		cout << "<ParkingData_Filename> should be simple txt file with lines of : id x1 y1 x2 y2 x3 y3 x4 y4" << endl;
		cout << "<ParkingSettings_Filename> should be simple txt file" << endl;
		cout << "<Video or Webcam> choose between video or Webcam" << endl;
		system("PAUSE");
		return -1;
	}

	roi_File_Name = string(argv[2]);
	settings_File_Name = string(argv[3]);

	get_roi_and_settings();

	if (string(argv[4]) == "Video" || string(argv[4]) == "Vid" || string(argv[4]) == "video" || string(argv[4]) == "vid" || string(argv[4]) == "V" || string(argv[4]) == "v")
	{
		vid.open(argv[1]);
	}
	else if (string(argv[4]) == "Webcam" || string(argv[4]) == "Web" || string(argv[4]) == "webcam" || string(argv[4]) == "web" || string(argv[4]) == "W" || string(argv[4]) == "w")
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

	system_timer.start();
	timer_save.start();
	detect_timer.start();
	ini_detect_timer.start();
	for (int i = 0; i < 16; i++)
	{
		parkir_time[i].start();
	}

	try
	{
		cout << endl << "Press 'h' for help" << endl;
		string::size_type pAt = settings_File_Name.find_last_of('.');                  // Find extension point
		const string videoOutFilename = settings_File_Name.substr(0, pAt) + "_out.avi";
		Size videoSize = Size((int)vid.get(CAP_PROP_FRAME_WIDTH), (int)vid.get(CAP_PROP_FRAME_HEIGHT));
		int ex = static_cast<int>(vid.get(CAP_PROP_FOURCC));
		outvideo.open(videoOutFilename, outvideo.fourcc('M', 'J', 'P', 'G'), vid.get(CAP_PROP_FPS), videoSize, true);

		while (true)
		{
			if (pause)
			{
				vid.read(frame);
				frame.copyTo(frame_copy);
				frame.copyTo(frame_display);
			}

			if (frame.empty())
			{
				cout << "Erro reading frame !" << endl;
				system("PAUSE");
				return -5;
			}

			int park_count = 0;
			if (process_place_bool)
			{
				process_place();
				int red_cout = 0;
				int green_cout = 0;

				if ((int)detect_timer.getTime() >= 1)
				{
					detect_timer.reset();
					temp_timer++;
					for (int &ids : contourIds_Place)
					{
						if (parking_Status[ids - 1])
						{
							temp_v[ids - 1]++;
						}
						else
						{
							temp_f[ids - 1]++;
						}
					}
					if (temp_timer == detect_time_val)
					{
						temp_timer = 0;
						for (int &ids : contourIds_Place)
						{
							if (temp_v[ids - 1] > temp_f[ids - 1])
							{
								parking_Status_temp[ids - 1] = true;
							}
							else
							{
								parking_Status_temp[ids - 1] = false;
							}
							temp_v[ids - 1] = 0;
							temp_f[ids - 1] = 0;
						}
					}
				}

				if ((int)ini_detect_timer.getTime() >= 5)
				{
					for (vector<Point2f>& points : contoursPoints_Place)
					{
						Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);
						string park_text = to_string(contourIds_Place[park_count]);
						Scalar color;

						int time_stamp = parkir_time[park_count].getTime();
						string time_string = "Time: ";
						time_string.append(to_string(time_stamp));

						if (parking_Status_temp[park_count] == true)
						{
							color = colorRed;
							red_cout++;
						}
						else
						{
							color = colorGreen;
							green_cout++;
						}

						putText(frame, park_text, Point(points.at(0).x - 10, (points.at(0).y + points.at(2).y) / 2), FONT_HERSHEY_COMPLEX, 0.5, color, 1, 8);
						putText(frame, time_string, Point(((points.at(0).x + points.at(1).x) / 2) - 25, (points.at(0).y + points.at(2).y) / 2), FONT_HERSHEY_COMPLEX, 0.5, color, 1, 8);
						rectangle(frame, cropRect, color, 1, 8);
						park_count++;
					}

					park_count = 0;
					for (int &ids : contourIds_Place)
					{
						bool parking_Status_Compare[16];

						if (parking_Status_temp[park_count] != parking_Status_Compare[park_count])
						{
							parkir_time[park_count].reset();
						}

						parking_Status_Compare[park_count] = parking_Status_temp[park_count];
						park_count++;
					}
					string park_text_ocp_1 = "Total de Vagas Livres: ";
					park_text_ocp_1.append(to_string(green_cout));
					string park_text_ocp_2 = "Total de Vagas Ocupadas: ";
					park_text_ocp_2.append(to_string(red_cout));
					string park_text_ocp_3 = "System Timer: ";
					park_text_ocp_3.append(to_string(system_timer.getTime()));
					putText(frame, park_text_ocp_1, Point(10, frame.size().height - 30), FONT_HERSHEY_COMPLEX, 0.6, colorGreen, 1, 8);
					putText(frame, park_text_ocp_2, Point(10, frame.size().height - 10), FONT_HERSHEY_COMPLEX, 0.6, colorRed, 1, 8);
					putText(frame, park_text_ocp_3, Point(frame.size().width - 200, 25), FONT_HERSHEY_COMPLEX, 0.5, colorWhite, 1, 8);
					imshow("Frame", frame);
				}
				else
				{
					syst_time_coutdown = 5 - ini_detect_timer.getTime();
					string park_text_ocp_1 = "Detection System Starting in: ";
					string park_text_ocp_2;
					park_text_ocp_2.append(to_string(syst_time_coutdown));
					park_text_ocp_2.append(" seconds");
					putText(frame, park_text_ocp_1, Point((frame.size().width / 2) - 175, frame.size().height / 2.5), FONT_HERSHEY_COMPLEX, 0.75, colorWhite, 1, 8);
					putText(frame, park_text_ocp_2, Point((frame.size().width / 2) - 175, frame.size().height / 2.2), FONT_HERSHEY_COMPLEX, 0.75, colorWhite, 1, 8);
					string park_text_ocp_3 = "System Timer: ";
					park_text_ocp_3.append(to_string(system_timer.getTime()));
					putText(frame, park_text_ocp_3, Point(frame.size().width - 200, 25), FONT_HERSHEY_COMPLEX, 0.5, colorWhite, 1, 8);
					imshow("Frame", frame);

					for (int i = 0; i < 16; i++)
					{
						parkir_time[i].reset();
					}
				}

				if (display)
				{
					namedWindow("Croped", WINDOW_GUI_NORMAL);
					imshow("Croped", display_roi());
				}
				else
				{
					destroyWindow("Croped");
				}

				if (display_process)
				{
					imshow("Processed Crop", display_roi_processed());
				}
				else
				{
					destroyWindow("Processed Crop");
				}
			}
			else
			{
				string out_text_1 = "System Offline!";
				putText(frame_copy, out_text_1, Point((frame.size().width / 2) - 165, frame.size().height / 2), FONT_HERSHEY_COMPLEX, 1, colorRed, 1, 8);
				ini_detect_timer.reset();
				for (vector<Point2f>& points : contoursPoints_Place)
				{
					Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);
					rectangle(frame, cropRect, colorWhite, 1, 8);
					string park_text = to_string(contourIds_Place[park_count]);
					putText(frame, park_text, Point(points.at(0).x - 10, (points.at(0).y + points.at(2).y) / 2), FONT_HERSHEY_COMPLEX, 0.5, colorWhite, 1, 8);
					park_count++;
				}

				string park_text_1 = "Total de Vagas: ";
				park_text_1.append(to_string(contourIds_Place.size()));
				putText(frame, park_text_1, Point(10, frame.size().height - 25), FONT_HERSHEY_COMPLEX, 0.75, colorWhite, 1, 8);
				string park_text_2 = "System Timer: ";
				park_text_2.append(to_string(system_timer.getTime()));
				putText(frame, park_text_2, Point(frame.size().width - 200, 25), FONT_HERSHEY_COMPLEX, 0.5, colorWhite, 1, 8);
				imshow("Frame", frame);
				for (int i = 0; i < 16; i++)
				{
					parkir_time[i].reset();
				}
				destroyWindow("Croped");
				destroyWindow("Processed Crop");
			}

			if ((int)timer_save.getTime() == 10)
			{
				timer_save.reset();

				if (process_place_bool)
				{
					save_Out_On();
				}
				else
				{
					save_Out_Off();
				}
			}

			if (writeVid)
			{
				outvideo.write(frame);
			}

			c = (char)waitKey(30);
			if (c == 27) {
				break;
			}
			switch (c)
			{
			case ' ':
				pause = !pause;
				break;
			case '1':
				process_place_bool = !process_place_bool;
				cout << "Process: " << process_place_bool << " !" << endl;
				break;
			case '2':
				display = !display;
				break;
			case '3':
				display_process = !display_process;
				break;
			case 'h':
				help();
				break;
			case 's':
				writeVid = !writeVid;
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

	for (int i = 0; i < 16; i++)
	{
		parkir_time[i].stop();
	}
	system_timer.stop();

	vid.release();
	destroyAllWindows();
	system("PAUSE");
	return 0;
}

void help()
{
	cout << endl << "space = play or pause video" << endl;
	cout << "Esc = exit aplication" << endl;
	cout << "1 = Start/pause place processing" << endl;
	cout << "2 = Display Roi Selections" << endl;
	cout << "3 = Display Contours Count" << endl;
}

Mat display_roi()
{
	int i = 0;
	offset = 10;
	Mat emptyMat = Mat::zeros(Size(150, 150), CV_8UC3);

	for (vector<Point2f>& points : contoursPoints_Place)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);

		Mat crop = frame_display(cropRect);
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

	for (vector<Point2f>& points : contoursPoints_Place)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);

		Mat crop = frame_display(cropRect);
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
	GaussianBlur(gray, blur, Size(blur_Val, blur_Val), 0, 0);
	threshold(blur, thresh, thresh_Contrast_Val, 200, THRESH_TRUNC);
	return thresh;
}

Mat processFrame(Mat resized, int i, vector<Point2f>& points)
{
	Mat thresh, morph, canny;
	Mat contoursMatrix = Mat::zeros(resized.size(), resized.type());

	Canny(resized, canny, thresh_Canny_Val, thresh_Canny_Val * 3, 3);

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

void save_Out_On()
{
	out_file.open(out_File_Name, ifstream::out | ifstream::trunc);
	if (!out_file.is_open() || out_file.fail())
	{
		out_file.close();
		cout << "Error : failed to erase file content !" << endl;
	}
	out_file.close();

	out_file.open(out_File_Name);

	if (!out_file.is_open() || out_file.fail())
	{
		out_file.close();
		cout << "Error : failed to open file content !" << endl;
	}

	out_file << "<table>" << endl;
	out_file << "<thead>" << endl;
	out_file << "<tr>" << endl;
	out_file << "<th>Vaga</th>" << endl;
	out_file << "<th>Status</th>" << endl;
	out_file << "<th>Time</th>" << endl;
	out_file << "</tr>" << endl;
	out_file << "<tbody>" << endl;

	for (int i = 1; i <= 16; i++)
	{
		out_file << "<tr>" << endl;
		out_file << "<td>" << to_string(i) << "</td>" << endl;
		out_file << "<td>" << parking_Status_temp[i] << "</td>" << endl;
		out_file << "<td>" << parkir_time[i].getTime() << "</td>" << endl;
		out_file << "</tr>" << endl;
	}

	out_file << "</tbody>" << endl;
	out_file << "</table>" << endl;

	out_file.close();

	imwrite("frame_out.jpeg", frame);

	cout << "Out files saved [ON]" << endl;
}

void save_Out_Off()
{
	out_file.open(out_File_Name, ifstream::out | ifstream::trunc);
	if (!out_file.is_open() || out_file.fail())
	{
		out_file.close();
		cout << "Error : failed to erase file content !" << endl;
	}
	out_file.close();

	out_file.open(out_File_Name);

	if (!out_file.is_open() || out_file.fail())
	{
		out_file.close();
		cout << "Error : failed to open file content !" << endl;
	}

	out_file << "<table>" << endl;
	out_file << "<thead>" << endl;
	out_file << "<tr>" << endl;
	out_file << "<th>Vaga</th>" << endl;
	out_file << "<th>Status</th>" << endl;
	out_file << "<th>Time</th>" << endl;
	out_file << "</tr>" << endl;
	out_file << "<tbody>" << endl;

	for (int i = 1; i <= 16; i++)
	{
		out_file << "<tr>" << endl;
		out_file << "<td>" << to_string(i) << "</td>" << endl;
		out_file << "<td>" << parking_Status_temp[i] << "</td>" << endl;
		out_file << "<td>" << parkir_time[i].getTime() << "</td>" << endl;
		out_file << "</tr>" << endl;
	}
	out_file << "</tbody>" << endl;
	out_file << "</table>" << endl;

	out_file.close();

	imwrite("frame_out.jpeg", frame_copy);

	cout << "Out files saved [OFF]" << endl;
}

void process_place()
{
	int i = 0;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	GaussianBlur(frame_gray, blur_Frame, Size(blur_Val, blur_Val), 0, 0, 4);
	threshold(blur_Frame, thresh_Contrast, thresh_Contrast_Val, 255, THRESH_TRUNC);
	Canny(thresh_Contrast, thresh_Canny, thresh_Canny_Val, thresh_Canny_Val * 2, 3);

	for (vector<Point2f>& points : contoursPoints_Place)
	{
		Rect cropRect(points.at(0).x + offset, points.at(0).y - (offset / 2), (points.at(1).x - points.at(0).x) - offset, (points.at(3).y - points.at(0).y) - offset);
		Mat crop = thresh_Canny(cropRect);

		Mat contoursMatrix = Mat::zeros(crop.size(), crop.type());

		vector<vector<Point> > contours_Roi;
		findContours(crop, contours_Roi, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<vector<Point> > areaContours;
		for (vector<Point> &c : contours_Roi)
		{
			if (contourArea(c) > 4.5)
			{
				areaContours.push_back(c);
			}
		}

		if (areaContours.size() > 2)
		{
			parking_Status[i] = true;
		}
		else
		{
			parking_Status[i] = false;
		}

		contours_Roi.clear();
		areaContours.clear();
		i++;
	}
}

void get_roi_and_settings() {
	roi_File.open(roi_File_Name);

	if (!roi_File.is_open() || roi_File.fail())
	{
		roi_File.close();
		cout << "Error : failed to open file content !" << endl;
	}

	while (getline(roi_File, roi_line_File))
	{
		istringstream iss(roi_line_File);
		float x1, y1, x2, y2, x3, y3, x4, y4;
		int id;

		iss >> id >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;

		Points_Place[0] = Point2f(x1, y1);
		Points_Place[1] = Point2f(x2, y2);
		Points_Place[2] = Point2f(x3, y3);
		Points_Place[3] = Point2f(x4, y4);

		contoursPoints_Place.push_back(Points_Place);

		contourIds_Place.push_back(id);

		cout << "Getting rois... [" << id << "]." << endl;
	}

	roi_File.close();

	setting_File.open(settings_File_Name);

	if (!setting_File.is_open() || setting_File.fail())
	{
		setting_File.close();
		cout << "Error : failed to open file content !" << endl;
	}

	int temp_tick = 0;
	int temp_Val[4];

	while (getline(setting_File, settings_line_File))
	{
		istringstream iss(settings_line_File);

		iss >> temp_Val[temp_tick];
		temp_tick++;

		cout << "Getting settings... [" << temp_tick << "]." << endl;
	}
	blur_Val = temp_Val[0];
	thresh_Contrast_Val = temp_Val[1];
	thresh_Canny_Val = temp_Val[2];
	detect_time_val = temp_Val[3];

	roi_File.close();

	if (contoursPoints_Place.size() <= 0)
	{
		cout << "Error Reading Data File !" << endl;
		system("PAUSE");
	}

	if (blur_Val < 0 || thresh_Contrast_Val < 0 || detect_time_val < 0 || thresh_Canny_Val < 0)
	{
		cout << "Error Reading Settings File !" << endl;
		system("PAUSE");
	}
}