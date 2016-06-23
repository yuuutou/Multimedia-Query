#include "Comparator.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#define INF 32767
#define SIM_NUM 450

using namespace std;

Comparator::Comparator(){
	files[0] = "flowers";
	files[1] = "interview";
	files[2] = "movie";
	files[3] = "musicvideo";
	files[4] = "sports";
	files[5] = "starcraft";
	files[6] = "traffic";
	scaling_factor = -1;
}

void Comparator::imgCompare(string vidPath, char* imgPath, int gap, string outFile, double* sim, double& max){
	string desFile = "descriptors\\"+vidPath + ".des";
	string testFile = string(imgPath) + ".des";
	//char histDes[600][24], layoutDes[600][48], audioDes[600][8], motionDes[600];
	//char qHistDes[150][24], qLayoutDes[150][48], qAudioDes[150][8], qMotionDes[150];

	readData(desFile, testFile);
	/*int temp = 0;
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
	}*/

	//string outFile = "output.txt";
	remove(outFile.c_str());
	double qFrame[4];
	/*temp = 0;
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
	}*/

	int pCount = 0;
	max = -1;
	double D = 0;

	for(int step=0;step<450;step+=gap){

		double avgSum = 0;
		double avgPct = 0;

		for(int i=0,j=step;i<150;i+=gap,j+=gap){

			qFrame[0] = simpleSum(queryData[i].histDes, fileData[j].histDes);
			qFrame[1] = simpleSum(queryData[i].layoutDes, fileData[j].layoutDes);
			qFrame[2] = simpleSum(queryData[i].soundDes, fileData[j].soundDes);
			qFrame[3] = simpleSum(queryData[i].motionDes, fileData[j].motionDes);

			double summation = compareSums(qFrame);
			double percent = D/(D+summation)*100;

			if (scaling_factor < 0)
				scaling_factor = findScale(summation, scaling_factor);
			////if (summation <= 0)
			////percent = 100.00;
			//else
			//{
				D = pow(10.0, scaling_factor*2 + 5);
				percent = ((double) D / (D + pow(summation,3))) * 100;
			//}
			avgSum += summation, avgPct += percent;

			

		}
		avgSum /= (150 / gap);
		avgPct /= (150 / gap);
		//percentages[pCount] = avgPct;
		sim[pCount] = avgPct;
		pCount++;

		if(max<avgPct)
			max = avgPct;

		/*std::stringstream str1, str2;
		str1 << fixed << setprecision(5) << avgPct;
		str2 << fixed << setprecision(2) << avgSum;
		stringstream ss;
		ss<<step;
		string s1 = ss.str();
		int temp = step+150;
		ss.str("");
		ss.clear();
		ss<<temp;
		string s2 = ss.str();
		writeOutput(outFile, "Starting Frame: " + s1 + " Ending Frame: " + s2 + " -- " + str1.str() + "%, average summation = " + str2.str());*/

	}
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
	ifstream ifile(qFileName, ios::in | ios::binary);
	if (!ifile)
	{
		//cout << "Opening file failed 1." << endl;
		return false;
	}
	else
	{
		ifile.read((char*)&queryData, sizeof(queryData));
	}
	ifile.close();

	// Read the current video file descriptor
	ifstream jfile(fileName, ios::in | ios::binary);
	if (!jfile)
	{
		//cout << "Opening file failed 2." << endl;
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

double Comparator::simpleSum(unsigned char* list1, unsigned char* list2)
{
	double sum = 0;
	for (int i = 0; i < sizeof(list1); i++)
	{
		//double x = abs((double)list1[i]), y = abs((double)list2[i]);
		double dis =  abs(list1[i]-list2[i]);
		sum += dis*dis;
	}
	return sum;
}

double Comparator::simpleSum(short* list1, short* list2){
	double sum = 0;
	for (int i = 0; i < sizeof(list1); i++)
	{
		//double x = abs((double)list1[i]), y = abs((double)list2[i]);
		double dis =  abs(list1[i]-list2[i]);
		sum += dis;
	}
	return sum;
}

double Comparator::simpleSum(unsigned char motion1, unsigned char motion2){
	double dis = abs(motion1 - motion2);
	return dis*dis;
}

double Comparator::compareSums(double* qFrame)
{	
	double a = qFrame[0];
	double b = qFrame[1];
	double c = qFrame[2];
	double d = qFrame[3];
	return qFrame[0]+qFrame[1]+qFrame[2]+qFrame[3];
}

string Comparator::getFileName(int index){
	return files[index];
}