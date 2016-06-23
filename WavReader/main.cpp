#include "WAV.h"

int main(){
	WAV wav;
	wav.readWAV("movie.wav");
	wav.showHead();
	wav.showData(10000,10100);
	return 0;
}