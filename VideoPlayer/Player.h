#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#define HEIGHT 288
#define WIDTH 352

using namespace cv;
using namespace std;

class Player{
public:
	Player();
	~Player();
	void playDir(char* path);
private:
	bool readFrame(string filename);
	void increaseFilenum(string& filenum);
	char* Rbuf;
	char* Gbuf;
	char* Bbuf;
	Mat curFrame;
};