#pragma once

#include <string>

typedef struct{
	char head[4];//"RIFF"
	int size;//file size
	char fmt[8];//"WAVEfmt "
	int fmtChunkSize;//size of the fmt chunk, PCM is 16
	short type;//Type of format (1 is PCM)
	short numChannels;//number of channels
	int sampleRate;//44100 CD 48000 DAT
	int dataPerSecond;//(Sample Rate * BitsPerSample * Channels) / 8
	short blockSize;//(BitsPerSample * Channels) / 8
	short bitsPerSample;//16 or 8. In the given database this value is always 16
	char dataHead[4];//"data"
	int dataChunkSize;//size of the data section
}Header;

class WAV{
public:
	WAV();
	~WAV();
	bool readWAV(string path);
	void showHead();
	void showData(int from, int to);
	Header header;
	short* data;
};