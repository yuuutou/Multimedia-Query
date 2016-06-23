#pragma once

#include <vector>

#define HEIGHT 288
#define WIDTH 352

using namespace std;

typedef struct{
	unsigned char histDes[24];
	unsigned char layoutDes[48];
	short soundDes[4];
	unsigned char motionDes;
}Des;

class Comparator{
public:
	Comparator();
	void imgCompare(string vidPath, char* imgPath, int gap, string outFile, double* sim, double& max);
	//void videoCompare(char* vidPath, char* vidCompPath, int gap);
	string getFileName(int index);
private:
	double compareSums(double* oFrame);
	double simpleSum(unsigned char* list1, unsigned char* list2);
	double simpleSum(short* list1, short* list2);
	double simpleSum(unsigned char motion1, unsigned char motion2);
	bool readData(string fileName, string qFileName);
	void writeOutput(string filename, string data);
	int findScale(double sum, int factor);
	//char querryData[81 * 150];
	//char fileData[81 * 600];
	//double percentages[450];
	Des queryData[150];
	Des fileData[600];
	string files[7];
	int scaling_factor;
};