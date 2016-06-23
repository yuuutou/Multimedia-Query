#include "Player.h"

using namespace std;

Player::Player(){
	Rbuf = new char[HEIGHT*WIDTH];
	Gbuf = new char[HEIGHT*WIDTH];
	Bbuf = new char[HEIGHT*WIDTH];
}

Player::~Player(){
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	if(curFrame.data)
		curFrame.release();
}

void Player::playDir(char* path){
	namedWindow( "VideoPlayer",WINDOW_AUTOSIZE);
	string filenum = "001";
	String s = string(path)+"\\"+string(path)+".wav";
	PlaySound(wstring(s.begin(), s.end()).c_str(), NULL, SND_ASYNC);
	while(readFrame(string(path)+"\\"+path+filenum+".rgb")){
		imshow("VideoPlayer",curFrame);
        cvWaitKey(31);
		increaseFilenum(filenum);
	}
	destroyWindow("VideoPlayer");
}

bool Player::readFrame(string filename){
	ifstream ifile(filename,ios::in|ios::binary);
	if(!ifile){
		//cout<<"Opening file failed."<<endl;
		return false;
	}	
	ifile.read(Rbuf,HEIGHT*WIDTH);
	ifile.read(Gbuf,HEIGHT*WIDTH);
	ifile.read(Bbuf,HEIGHT*WIDTH);
	ifile.close();

	curFrame.create(HEIGHT,WIDTH,CV_8UC3);
	uchar* p;
	for(int i=0;i<HEIGHT;i++){
		p = curFrame.ptr<uchar>(i);
		for(int j=0;j<WIDTH;j++){
			p[j*3] = Bbuf[i*WIDTH+j];
			p[j*3+1] = Gbuf[i*WIDTH+j];
			p[j*3+2] = Rbuf[i*WIDTH+j];
		}
	}

	return true;
}

void Player::increaseFilenum(string& filenum){
	if(filenum[2]++=='9'){
		filenum[2] = '0';
		if(filenum[1]++=='9'){
			filenum[1] = '0';
			filenum[0]++;
		}
	}
}