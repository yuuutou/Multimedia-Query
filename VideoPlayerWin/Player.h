#pragma once
#pragma region Includes and Manifest Dependencies

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "resource.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <shobjidl.h>
#include <Shlobj.h>
#include <stdio.h>

#define HEIGHT 288
#define WIDTH 352
#define QUERY_LENGTH 150
#define DATABASE_LENGTH 600
#define MAX_PATH_LENGTH 20
#define GAP 1
#define SIM_NUM 450
#define FILE_NUM 7

using namespace cv;
using namespace std;

class Player{
public:
	Player(string s);
	~Player();
	void init(HWND handle, char* windowName, int parentID);
	void playDir(HWND hprogress, string path, int videoLength, char* windowName);
	void setPause();
	void continuePlay();
	bool isPause();
	void stopPlay();
	void createSimPic(int index, double* sims);
	void reset();
	Mat getSimPic(int index);
	Mat getFirstQuery(char* path);
	Mat getFirstResult(string path);
private:
	string name;
	bool readFrame(string filename);
	void increaseFilenum(string& filenum);
	string computeFilenum(int num);
	string buildFileName(string filenum, char winName, string path);
	char* Rbuf;
	char* Gbuf;
	char* Bbuf;
	Mat curFrame;
	int nextFrame;
	bool pause;
	bool start;
	int curPos;
	Mat simPic[FILE_NUM];
};