#include "Comparator.h"

using namespace std;

int main( int argc, char** argv ){
	Comparator comp;
	char* files[] = { "flowers", "interview", "movie", "musicvideo", "sports", "starcraft", "traffic" };
	string outFile;
	int gap = 10;
	double* percentages;
	for (int i = 0; i < 1; i++)
	{
		
		// Get the query comparison data for the selected files
		//outFile = string(files[i]) + "-output.txt";
		percentages = comp.imgCompare(files[i], "first", gap, outFile);
		for (int j = 0; j < 450/gap; j++)
		{
			cout << percentages[j] << endl;
		}
	}
    return 0;
}