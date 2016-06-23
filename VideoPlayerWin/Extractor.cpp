#include "Extractor.h"
#define INF 32767

using namespace std;

Extractor::Extractor(){
	Rbuf = new char[HEIGHT*WIDTH];
	Gbuf = new char[HEIGHT*WIDTH];
	Bbuf = new char[HEIGHT*WIDTH];
	wav = new WAV();
	previousMotionDist = -1;
	prepreMotionDist = -1;
	previousMotionInfo[0] = -1;
	previousMotionDes = -1;
	audioIndex = 0;
}

Extractor::~Extractor(){
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	delete wav;
}

void Extractor::extractDir(char* path, HWND hDlg, HWND hWait){
	string desFile = string(path)+".des";
	remove(desFile.c_str());
	//cout<<path<<endl;
	string filenum = "001";
	wav->readWAV(string(path)+"\\"+string(path)+".wav");
	//wav->showHead();
	nSamples = wav->header.sampleRate/30 * wav->header.numChannels;
	while(readFrame(string(path)+"\\"+path+filenum+".rgb")){
		//cout<<"Extracting: "<<path+filenum+".rgb"<<endl;
		string s = "Extracting: "+string(path)+filenum+".rgb";
		SetDlgItemText(hDlg,IDC_EDIT_LINE1,wstring(s.begin(), s.end()).c_str());
		SendMessage(hWait, PBM_STEPIT, 0, 0);
		UpdateWindow(hDlg);
		extractMotion();
		extractHist();
		extractLayout();
		extractSound();
		writeDes(desFile);
		increaseFilenum(filenum);
	}
	writeLastMotion(string(path)+".des");
	
}

void Extractor::increaseFilenum(string& filenum){
	if(filenum[2]++=='9'){
		filenum[2] = '0';
		if(filenum[1]++=='9'){
			filenum[1] = '0';
			filenum[0]++;
		}
	}
}

bool Extractor::readFrame(string filename){
	ifstream ifile(filename,ios::in|ios::binary);
	if(!ifile){
		//cout<<"Opening file failed."<<endl;
		return false;
	}	
	ifile.read(Rbuf,HEIGHT*WIDTH);
	ifile.read(Gbuf,HEIGHT*WIDTH);
	ifile.read(Bbuf,HEIGHT*WIDTH);
	ifile.close();
	return true;
}

void Extractor::extractHist(){
	//initialize hist buf
	int hist[24];
	for(int i=0;i<24;i++)
		hist[i] = 0;
	//histograms for RGB channels
	for(int i=0;i<HEIGHT*WIDTH;i++){
		hist[(unsigned char)Rbuf[i]/32]++;
		hist[(unsigned char)Gbuf[i]/32+8]++;
		hist[(unsigned char)Bbuf[i]/32+16]++;
	}
	//quantize to 256 levels
	for(int i=0;i<24;i++){
		histDes[i] = (char)(hist[i]/(WIDTH*HEIGHT/256+1));
	}
}

void Extractor::extractLayout(){
	//initialize the loc buf
	int loc[48];
	for(int i=0;i<48;i++)
		loc[i] = 0;
	//calculate the loc buf
	//divide each channel to 4*4 blocks, compute mean
	for(int i=0;i<HEIGHT*WIDTH;i++){
		loc[(i/WIDTH/(HEIGHT/4))*4+(i%WIDTH/(WIDTH/4))] += (unsigned char)Rbuf[i];
		loc[(i/WIDTH/(HEIGHT/4))*4+(i%WIDTH/(WIDTH/4))+16] += (unsigned char)Gbuf[i];
		loc[(i/WIDTH/(HEIGHT/4))*4+(i%WIDTH/(WIDTH/4))+32] += (unsigned char)Bbuf[i];
	}
	for(int i=0;i<48;i++){
		layoutDes[i] = (char)(loc[i]/(WIDTH*HEIGHT/16));
	}
}

void Extractor::extractMotion(){
	//initialize the motion info buf for the current frame
	int motionInfo[192];
	for(int i=0;i<192;i++)
		motionInfo[i] = 0;
	//calculate the motion info for the current frame
	for(int i=0;i<HEIGHT*WIDTH;i++){
		motionInfo[(i/WIDTH/(HEIGHT/8))*8+(i%WIDTH/(WIDTH/8))] += (unsigned char)Rbuf[i];
		motionInfo[(i/WIDTH/(HEIGHT/8))*8+(i%WIDTH/(WIDTH/8))+64] += (unsigned char)Gbuf[i];
		motionInfo[(i/WIDTH/(HEIGHT/8))*8+(i%WIDTH/(WIDTH/8))+128] += (unsigned char)Bbuf[i];
	}
	for(int i=0;i<192;i++)
		motionInfo[i] /= (WIDTH*HEIGHT/64);
	//calculate the previous motion distance
	if(previousMotionInfo[0]>=0){//if it is not the first frame
		previousMotionDist = 0;
		for(int i=0;i<192;i++)
			previousMotionDist += abs(motionInfo[i]-previousMotionInfo[i]);
		previousMotionDist /= 192;
		if(prepreMotionDist<0)//if it is the second frame
			previousMotionDes = (char)previousMotionDist;
		else
			previousMotionDes = (char)(previousMotionDist+prepreMotionDist)/2;
	}
	for(int i=0;i<192;i++)
		previousMotionInfo[i] = motionInfo[i];
	prepreMotionDist = previousMotionDist;
}

void Extractor::writeDes(string filename){
	ofstream ofile(filename, ios::out | ios::app | ios::binary);
	if(!ofile){
		cout<<"Opening file failed."<<endl;
		return;
	}
	if(previousMotionDes>=0){
		//cout<<(int)previousMotionDes;
		ofile.write(&previousMotionDes,1);
	}
	/*for(int i=0;i<24;i++)
		cout<<(int)histDes[i]<<" ";
	cout<<endl;*/
	ofile.write(histDes,24);
	/*for(int i=0;i<48;i++)
		cout<<(int)layoutDes[i]<<" ";
	cout<<endl;*/
	ofile.write(layoutDes,48);
	/*for(int i=0;i<4;i++)
		cout<<audioDes[i]<<" ";
	cout<<endl*/;
	ofile.write((char*)audioDes,8);
	ofile.close();
}

void Extractor::writeLastMotion(string filename){
	ofstream ofile(filename, ios::out | ios::app | ios::binary);
	if(!ofile){
		cout<<"Opening file failed."<<endl;
		return;
	}
	char motionDes = (char)previousMotionDist;
	//cout<<(int)motionDes;
	ofile.write(&motionDes,1);
	ofile.close();
}

void Extractor::extractSound(){
	//compute the max and min for each channel in the audio segment
	//there are two channels
	for(int j=0;j<2;j++){
		short max = -INF;
		short min = INF;
		int limit = audioIndex+nSamples<wav->header.dataChunkSize/2 ? audioIndex+nSamples : wav->header.dataChunkSize/2;
		for(int i=audioIndex+j;i<limit;i+=2){
			if(wav->data[i]>max)
				max = wav->data[i];
			if(wav->data[i]<min)
				min = wav->data[i];
		}
		audioDes[j*2] = max;//max for channel j
		audioDes[j*2+1] = min;//min for channel j
	}
	audioIndex += nSamples;
}