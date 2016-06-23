#pragma once

#include <vector>
#include <iostream>

#define HEIGHT 288
#define WIDTH 352

using namespace std;

class Comparator{
public:
	Comparator();
	double* imgCompare(char* vidPath, char* imgPath, int gap, string outFile);
	void videoCompare(char* vidPath, char* vidCompPath, int gap);
private:
	double compareSums(double* oFrame);
	double simpleSum(char* list1, char* list2);
	double simpleSum(int* list1, int* list2);
	bool readData(string fileName, string qFileName);
	void writeOutput(string filename, string data);
	int findScale(double sum, int factor);
	char querryData[81 * 150];
	char fileData[81 * 600];
	double percentages[450];
};