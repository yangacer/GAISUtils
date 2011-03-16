#include "fileUtils.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>
#include <iomanip>
#include <utility>

#define SEGMENT_SIZE 1024*1024*8

using namespace std;

spliter::spliter(unsigned int bufsize)
{
	// bounded by 8M buffer
	if(bufsize > SEGMENT_SIZE || bufsize==0)
		bufsize = SEGMENT_SIZE;
	buf_.resize(bufsize);
}

int spliter::operator()(vector<string>& result, unsigned int size, char const* filename)
{
	ifstream in(filename, ios_base::in | ios_base::binary);
	if(!in.is_open()){
		cerr<<"partition: Cannot open target file "<<filename<<endl;
		return 0;
	}
	
	//int oldsize = result.size();
	//int part = 1;

	// estimate partition number
	struct stat fileStat;
	stat(filename, &fileStat);
	float segNum = (float)fileStat.st_size / size;
	
	if(fileStat.st_size % size)	segNum++;
	
	int radixLen = (int)ceil(log10((float)segNum));
	stringstream sstr;
	ofstream out;

	int lastPartLen = 0;

	for(int i=0; i<segNum; i++){
		sstr.str("");
		sstr<<filename<<"-part-"<<setfill('0')<<setw(radixLen)<<i;
		
		out.open(sstr.str().c_str(), ios_base::out | ios_base::binary);
		if(!out.is_open()){
			cerr<<"partition: Open partition file error."<<endl;
			return 0;
		}

		int iterCount = (int)floorf(size / (float)buf_.capacity());
		for(int j=0; j< iterCount;j++){	
			buf_.tryfill(in);
			out<<buf_;
			buf_.seekoff(buf_.capacity());
		}
		int tailPart = size % buf_.capacity();
		if(tailPart){
			buf_.tryfill(in);
			out<<make_pair(buf_.begin(), buf_.begin() + tailPart);
			buf_.seekoff(tailPart);
		}
		out.close();
		result.push_back(sstr.str());
		if(i+1 == segNum)
			lastPartLen = iterCount * buf_.capacity() + tailPart;
	}

	buf_.clear();	

	return lastPartLen;
}

