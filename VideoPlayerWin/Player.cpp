#include "Player.h"

using namespace std;

Player::Player(string s){
	name = s;
	Rbuf = new char[HEIGHT*WIDTH];
	Gbuf = new char[HEIGHT*WIDTH];
	Bbuf = new char[HEIGHT*WIDTH];
	nextFrame = 1;
	pause = true;
	start = false;
	curPos = 0;
}

Player::~Player(){
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	if(curFrame.data)
		curFrame.release();
}

void Player::init(HWND handle, char* windowName, int parentID){
	Mat image;
    image = imread("starter.bmp", IMREAD_COLOR);
	namedWindow( windowName,WINDOW_AUTOSIZE);
	HWND hWnd2 = (HWND) cvGetWindowHandle(windowName);
	HWND hParent = ::GetParent(hWnd2); 
	::SetParent(hWnd2, GetDlgItem(handle, parentID)); 
	::ShowWindow(hParent, SW_HIDE);
	imshow( windowName, image );
}

void Player::playDir(HWND hprogress, string path, int videoLength, char* windowName){
	string filenum = computeFilenum(nextFrame);
	if(!start){
		start = true;
		string s;
		if(windowName[0]=='Q')
			s = "open "+path+"\\"+path+".wav alias "+name;
		else
			s = "open database\\"+path+"\\"+path+".wav alias "+name;
		mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
		s = "play "+name;
		mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
	}
	while(!pause&&readFrame(buildFileName(filenum,windowName[0],path))){
		int truePos = SendMessage(hprogress, TBM_GETPOS, 0, 0);
		if(curPos!=truePos){
			nextFrame = videoLength/100*truePos;
			stringstream ss;
			ss<<nextFrame*33;
			String s2 = "play "+name+" from " + ss.str();
			mciSendString(wstring(s2.begin(), s2.end()).c_str(),NULL,0,NULL);
		}
		SendMessage(hprogress, TBM_SETPOS, TRUE, nextFrame*100/videoLength);
		curPos = SendMessage(hprogress, TBM_GETPOS, 0, 0);
		imshow(windowName,curFrame);
        cvWaitKey(31);
		//increaseFilenum(filenum);
		nextFrame++;
		filenum = computeFilenum(nextFrame);
	}
	if(!pause){
		nextFrame=1;
		pause = true;
		start = false;
		stopPlay();
	}
}

void Player::setPause(){
	pause = true;
	string s = "pause "+name;
	mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
}

void Player::continuePlay(){
	pause = false;
	string s = "resume "+name;
	mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
}

void Player::stopPlay(){
	string s = "stop "+name;
	mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
	s = "close "+name;
	mciSendString(wstring(s.begin(), s.end()).c_str(),NULL,0,NULL);
}

bool Player::isPause(){
	return pause;
}

void Player::createSimPic(int index, double* sims){
	Size size (354,66);
	Mat temp;
	temp.create(50,2*SIM_NUM,CV_8UC3);
	uchar* p;
	for(int i=0;i<50;i++){
		p = temp.ptr<uchar>(i);
		for(int j=0;j<2*SIM_NUM;j++){
			p[j*3] = 100-2*i > sims[j/2] ? 244 : 0;
			p[j*3+1] = 100-2*i > sims[j/2] ? 244 : 0;
			p[j*3+2] = 100-2*i > sims[j/2] ? 244 : 255;
		}
	}
	cv::resize(temp,simPic[index],size);
}

void Player::reset(){
	stopPlay();
	nextFrame = 1;
	pause = true;
	start = false;
	curPos = 0;
}

Mat Player::getSimPic(int index){
	return simPic[index];
}

Mat Player::getFirstQuery(char* path){
	string s = string(path)+"\\"+path+"001.rgb";
	readFrame(s);
	return curFrame;
}

Mat Player::getFirstResult(string path){
	string s = "database\\"+path+"\\"+path+"001.rgb";
	readFrame(s);
	return curFrame;
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

string Player::computeFilenum(int num){
	char result[4];
	result[0] = num/100 + '0';
	result[1] = num/10%10 + '0';
	result[2] = num%10 + '0';
	result[3] = '\0';
	return string(result);
}

string Player::buildFileName(string filenum, char winName, string path){
	string filePath;
	if(winName=='Q')
		filePath = path+"\\"+path+filenum+".rgb";
	else
		filePath = "database\\"+path+"\\"+path+filenum+".rgb";
	return filePath;
}