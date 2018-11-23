#include "Place.h"

int main(int argc, char** argv) {
	if (argc != 3)
	{
		cout << "Usage: Place_Space.exe <Video_Filename or Picture_Filename or Camera_Number> <Video or Picture or Webcam>" << endl << endl;
		cout << "<Video_Filename or Picture_Filename> is the adress of the file" << endl;
		cout << "<Camera_Number> can be 0, 1, ... for attached cameras" << endl;
		cout << "<Video or Picture or Webcam> choose between video or picture or Webcam" << endl;
		system("PAUSE");
		return -1;
	}

	if (string(argv[2]) == "Video" || string(argv[2]) == "Vid" || string(argv[2]) == "video" || string(argv[2]) == "vid" || string(argv[2]) == "V" || string(argv[2]) == "v")
	{
		vid.open(argv[1]);
		if (!vid.isOpened())
		{
			cout << "Error loading video file !" << endl;
			system("PAUSE");
			return -11;
		}
		else
		{
			vid.read(src);
			src.copyTo(srcTemp);
		}

		if (src.empty())
		{
			cout << "Error loading frame in program !" << endl;
			system("PAUSE");
			return -12;
		}
	}
	else if (string(argv[2]) == "Webcam" || string(argv[2]) == "Web" || string(argv[2]) == "webcam" || string(argv[2]) == "web" || string(argv[2]) == "W" || string(argv[2]) == "w")
	{
		string String_temp = argv[1];
		int Int_temp = stoi(String_temp);
		vid.open(Int_temp);
		if (!vid.isOpened())
		{
			cout << "Error loading video file !" << endl;
			system("PAUSE");
			return -11;
		}
		else
		{
			vid.read(src);
			src.copyTo(srcTemp);
		}

		if (src.empty())
		{
			cout << "Error loading frame in program !" << endl;
			system("PAUSE");
			return -13;
		}
	}
	else if (string(argv[2]) == "Picture" || string(argv[2]) == "Pic" || string(argv[2]) == "picture" || string(argv[2]) == "pic" || string(argv[2]) == "P" || string(argv[2]) == "p")
	{
		src = imread(argv[1], 1);
		src.copyTo(srcTemp);
		if (src.empty())
		{
			cout << "Error loading picture !" << endl;
			system("PAUSE");
			return -21;
		}
	}
	else
	{
		cout << "<Video or Picture or Webcam> choose between video or picture or Webcam" << endl;
		system("PAUSE");
		return -2;
	}

	roi_File.open(roi_File_Name, ifstream::out | ifstream::trunc);
	if (!roi_File.is_open() || roi_File.fail())
	{
		roi_File.close();
		cout << "Error : failed to erase file content !" << endl;
	}
	roi_File.close();

	try
	{
		cout << "Press 'h' for help" << endl;

		namedWindow(nameWindow, WINDOW_AUTOSIZE);

		setMouseCallback(nameWindow, onMouseCallback, 0);

		while (true)
		{
			c = (char)waitKey(30);
			if (c == 27) {
				break;
			}
			switch (c)
			{
			case 'h':
				help();
				break;
			case ' ':
				if (vid.isOpened())
				{
					vid.read(src);
				}
				break;
			case '3':
				saveRoiOut();
				break;
			}

			if (points.size() == 4)
			{
				cout << "Confirmar posicao ?" << endl;

				while (true)
				{
					c = (char)waitKey(30);
					switch (c)
					{
					case '1':
						saved = true;
						break;
					case '2':
						deleted = true;
						break;
					}
					vector<vector<Vec2i>> z(1);
					z[0] = coordinates;

					if (saved)
					{
						drawContoursFunction(src, z, ids, colorWhite);

						cout << "id: " << ids << "\tcoordinates: " << finalCoordinateId[0] << endl;

						save_roi(coordinates, ids);

						ids += 1;

						points.clear();

						coordinates[0] = Point(0, 0);
						coordinates[1] = Point(0, 0);
						coordinates[2] = Point(0, 0);
						coordinates[3] = Point(0, 0);

						saved = false;
						src.copyTo(copySrc);
						break;
					}

					else if (deleted)
					{
						cout << "delete" << endl;

						points.clear();

						coordinates[0] = Point(0, 0);
						coordinates[1] = Point(0, 0);
						coordinates[2] = Point(0, 0);
						coordinates[3] = Point(0, 0);

						deleted = false;

						if (copySrc.empty())
						{
							srcTemp.copyTo(src);
						}
						else
						{
							copySrc.copyTo(src);
						}
						break;
					}
				}
			}
			imshow(nameWindow, src);
		}
	}
	catch (const std::exception&)
	{
		cout << "Error !" << endl;
		system("PAUSE");
		return -100;
	}

	if (vid.isOpened())
	{
		vid.release();
	}

	destroyAllWindows();
	system("PAUSE");
	return 0;
}

void help()
{
	cout << endl << "Space = Read new frame" << endl;
	cout << "1 = Save selection" << endl;
	cout << "2 = Delete selection" << endl;
	cout << "3 = Save roi .txt file" << endl;
	cout << "Esc = Exit aplication" << endl;
}

void onMouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (points.size() == 4)
	{
		return;
	}
	else
	{
		Point seed = Point(x, y);
		Point seedRect = Point(x + 20, y + 20);
		Rect rect(x - 2, y - 2, 4, 4);

		squareRect = rect;

		if (event == EVENT_LBUTTONDOWN)
		{
			clickCount += 1;
			cout << "Left button of the mouse is clicked - position " << seed << "\t clickCount: " << clickCount << endl;
			coordinates[clickCount - 1] = seed;
			points.push_back(seed);

			if (clickCount >= 4)
			{
				handleDone();
			}
			else if (clickCount > 0)
			{
				handleClickProgress();
			}
		}
		else if (event != EVENT_LBUTTONDOWN)
			return;
	}
}

void handleClickProgress()
{
	rectangle(src, squareRect, colorGreen, -1, 8, 0);
	imshow(nameWindow, src);
	if (clickCount > 2)
	{
		line(src, coordinates[1], coordinates[2], colorBlue, 2);
		imshow(nameWindow, src);
	}

	else if (clickCount > 1)
	{
		line(src, coordinates[0], coordinates[1], colorBlue, 2);
		imshow(nameWindow, src);
	}
}

void handleDone()
{
	line(src, coordinates[2], coordinates[3], colorBlue, 2);
	line(src, coordinates[3], coordinates[0], colorBlue, 2);
	imshow(nameWindow, src);

	clickCount = 0;
}

void drawContoursFunction(Mat image, vector<vector<Vec2i>> coordinates, int label, Scalar color)
{
	drawContours(image, coordinates, -1, color, 2, 8);

	vector<Moments> mu(coordinates.size());
	for (int i = 0; i < coordinates.size(); i++)
	{
		mu[i] = moments(coordinates[i], false);
	}

	vector<Point2f> mc(coordinates.size());
	for (int i = 0; i < coordinates.size(); i++)
	{
		mc[i] = Point2d(mu[i].m10 / mu[i].m00 - 10, mu[i].m01 / mu[i].m00 + 3);
		finalCoordinateId[i] = mc[i];
		putText(image, to_string(label), mc[i], FONT_HERSHEY_SIMPLEX, 0.8, colorRed, 1, 8);
	}
	imshow(nameWindow, src);
}

void save_roi(vector<Vec2i> cord, int id) {
	roi_File.open(roi_File_Name, ios::app);

	cout << ids << " ";
	roi_File << ids << " ";

	for (Point& p : points)
	{
		cout << p.x << " " << p.y << " ";
		roi_File << p.x << " " << p.y << " ";
	}

	cout << endl << "Done !" << endl;
	roi_File << endl;
	roi_File.close();
}

void saveRoiOut()
{
	file.open(roi_File_Name_Out, ifstream::out | ifstream::trunc);
	string textString = "";

	if (!file.is_open() || file.fail())
	{
		file.close();
		printf("\nError : failed to erase file content !");
	}
	file.close();

	file.open(roi_File_Name_Out, ios::app);
	roi_File.open(roi_File_Name);

	while (getline(roi_File, textString))
	{
		istringstream iss(textString);
		int id, x1, y1, x2, y2, x3, y3, x4, y4;

		iss >> id >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
		file << id << " " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << x3 << " " << y3 << " " << x4 << " " << y4 << endl;
	}
	file.close();
	roi_File.close();

	cout << "File Copied !" << endl;
}