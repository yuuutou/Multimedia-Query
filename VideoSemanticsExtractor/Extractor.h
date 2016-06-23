#pragma once

#include <vector>
#include "WAV.h"

#define HEIGHT 288
#define WIDTH 352

using namespace std;

class Extractor{
public:
	Extractor();
	~Extractor();
	void extractDir(char* path);
private:
	bool readFrame(string filename);
	void extractHist();
	void extractLayout();
	void extractMotion();
	void writeDes(string filename);
	void extractSound();
	void increaseFilenum(string& filenum);
	void writeLastMotion(string filename);
	char* Rbuf;
	char* Gbuf;
	char* Bbuf;
	int previousMotionInfo[192];
	char histDes[24];
	char layoutDes[48];
	char previousMotionDes;
	short audioDes[4];
	int previousMotionDist;
	int prepreMotionDist;
	int nSamples;
	int audioIndex;
	WAV* wav;
};