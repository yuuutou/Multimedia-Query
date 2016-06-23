#include "Comparator.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#define INF 32767

using namespace std;

Comparator::Comparator(){
}

double* Comparator::imgCompare(char* vidPath, char* imgPath, int gap, string outFile){
	string desFile = string(vidPath) + ".des";
	string testFile = string(imgPath) + ".des";
	char histDes[600][24], layoutDes[600][48], audioDes[600][8], motionDes[600];
	char qHistDes[150][24], qLayoutDes[150][48], qAudioDes[150][8], qMotionDes[150];

	readData(desFile, testFile);
	int temp = 0;
	for (int j = 0; j < 150; j += gap)
	{
		for (int i = temp; i < (temp + 81) - 1; i++)
		{
			if (i >= temp && i < temp + 24)
				qHistDes[j][i - temp] = querryData[i];
			else if (i >= temp + 24 && i < temp + 72)
				qLayoutDes[j][i - (temp + 24)] = querryData[i];
			else if (i >= temp + 72 && i < temp + 81)
				qAudioDes[j][i - (temp + 72)] = querryData[i];
			else if (i == temp + 80)
				qMotionDes[j] = querryData[i];
		}
		temp += 81;
	}

	//string outFile = "output.txt";
	remove(outFile.c_str());
	double qFrame[4];
	temp = 0;
	for (int j = 0; j < 600; j += gap)
	{
		for (int i = temp; i < (temp+81)-1; i++)
		{
			if (i >= temp && i < temp+24)
				histDes[j][i-temp] = fileData[i];
			else if (i >= temp+24 && i < temp+72)
				layoutDes[j][i - (temp + 24)] = fileData[i];
			else if (i >= temp+72 && i < temp+81)
				audioDes[j][i - (temp + 72)] = fileData[i];
			else if (i == temp+80)
				motionDes[j] = fileData[i];
		}
		temp += 81;
	}
	int scaling_factor = -1;
	int pCount = 0;
	double D = 0;
	for (int x = 0; (x+150) < 600; x+=gap)
	{
		double summation = 0, percent = 0, avgSum = 0, avgPct = 0;
		for (int y = 0; y < 150; y += gap)
		{
			qFrame[0] = simpleSum(qHistDes[y], histDes[x]);
			qFrame[1] = simpleSum(qLayoutDes[y], layoutDes[x]);
			qFrame[2] = simpleSum(qAudioDes[y], audioDes[x]);
			qFrame[3] = abs(abs((double)qMotionDes[y]) - abs((double)motionDes[x]));

			summation = compareSums(qFrame);
			if (scaling_factor < 0)
				scaling_factor = findScale(summation, scaling_factor);
			if (summation <= 0)
				percent = 100.00;
			else
			{
				D = pow(10.0, scaling_factor + 1);
				percent = ((double) D / (D + summation)) * 100;
			}
			avgSum += summation, avgPct += percent;
			
		}
		avgSum /= (150 / gap);
		avgPct /= (150 / gap);
		percentages[pCount] = avgPct;
		pCount++;
		/*std::stringstream str1, str2;
		str1 << fixed << setprecision(5) << avgPct;
		str2 << fixed << setprecision(2) << avgSum;
		std::string s1 = std::to_string(x);
		std::string s2 = std::to_string(x+150);
		writeOutput(outFile, "Starting Frame: " + s1 + " Ending Frame: " + s2 + " -- " + str1.str() + "%, average summation = " + str2.str());*/
	}
	return percentages;
}

int Comparator::findScale(double sum, int factor)
{
	bool flag = true;
	int scale = 0;
	while (sum > 10)
	{
		sum /= 10;
		scale++;
		flag = false;
	}
	if (factor < 0 && flag)
	{
		return 0;
	}
	return scale;
}

bool Comparator::readData(string fileName, string qFileName)
{
	// Read the query video file descriptor
	ifstream ifile(qFileName, ios::in | ios::app | ios::binary);
	if (!ifile)
	{
		cout << "Opening file failed 1." << endl;
		return false;
	}
	else
	{
		ifile.read((char*)&querryData, sizeof(querryData));
	}
	ifile.close();

	// Read the current video file descriptor
	ifstream jfile(fileName, ios::in | ios::app | ios::binary);
	if (!jfile)
	{
		cout << "Opening file failed 2." << endl;
		return false;
	}
	else
	{
		jfile.read((char*)&fileData, sizeof(fileData));
	}
	jfile.close();

	return true;
}

void Comparator::writeOutput(string filename, string data){
	std::fstream fs;
	fs.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

	fs << data << endl;

	fs.close();
}

double Comparator::simpleSum(char* list1, char* list2)
{
	double sum = 0;
	for (int i = 0; i < sizeof(list1); i++)
	{
		double x = abs((double)list1[i]), y = abs((double)list2[i]);
		sum += abs(x-y);
	}
	return sum;
}

double Comparator::compareSums(double* qFrame)
{
	double sums = 0;
	for (int i = 0; i<4; i++){
		sums += qFrame[i];// *pow(2, i);
	}
	return sums;
}