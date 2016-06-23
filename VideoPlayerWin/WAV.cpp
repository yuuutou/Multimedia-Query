#include "WAV.h"

using namespace std;

WAV::WAV(){
	data = 0;
}

WAV::~WAV(){
	if(data)
		delete data;
}

bool WAV::readWAV(string path){
	ifstream ifile(path,ios::in|ios::binary);
	if(!ifile){
		//cout<<"Opening file failed."<<endl;
		return false;
	}	
	ifile.read((char*)&header,sizeof(header));
	if(data)
		delete data;
	data = new short[header.dataChunkSize/2];
	ifile.read((char*)data,header.dataChunkSize);
	ifile.close();
	return true;
}

void WAV::showHead(){
	for(int i=0;i<4;i++)
		cout<<header.head[i];
	cout<<endl;
	cout<<"file size: "<<header.size<<endl;
	for(int i=0;i<8;i++)
		cout<<header.fmt[i];
	cout<<endl;
	cout<<"fmtChunkSize: "<<header.fmtChunkSize<<endl;
	cout<<"type: "<<header.type<<endl;
	cout<<"numChannels: "<<header.numChannels<<endl;
	cout<<"sampleRate: "<<header.sampleRate<<endl;
	cout<<"data size per second: "<<header.dataPerSecond<<endl;
	cout<<"blockSize: "<<header.blockSize<<endl;
	cout<<"bitsPerSample: "<<header.bitsPerSample<<endl;
	for(int i=0;i<4;i++)
		cout<<header.dataHead[i];
	cout<<endl;
	cout<<"dataChunkSize: "<<header.dataChunkSize<<endl;
}

void WAV::showData(int from, int to){
	cout<<"channel 1: "<<endl;
	for(int i=from;i<to;i+=2)
		cout<<data[i]<<" ";
	cout<<endl;
	cout<<"channel 2: "<<endl;
	for(int i=from+1;i<to;i+=2)
		cout<<data[i]<<" ";
	cout<<endl;
}