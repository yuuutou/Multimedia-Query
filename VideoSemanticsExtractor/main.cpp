#include "Extractor.h"

using namespace std;

int main( int argc, char** argv ){
	Extractor ext;
	for(int i=1; i<argc;i++)
		ext.extractDir(argv[i]);
    return 0;
}