/*! \file container.cpp
 *  \brief Demostrate how to use circular_buffer.
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include <limits>
#include "container.h"

#define MB 1048576

int main(int argc, char** argv)
{
	using namespace std;

	if(argc<4){
		cerr<<"file_name buffer_size seek_offset"<<endl;
		exit(1);
	}

	ifstream fin(argv[1]);

	if(!fin.is_open()){
		cerr<<"open file fail"<<endl;
		exit(1);
	}

	int size = atoi(argv[2]);
	if(!size){
		cerr<<"buffer size cannot be zero"<<endl;
		exit(1);
	}

	circular_buffer<char> cb(size*MB);
	
	unsigned long readCount;
	unsigned long gcount = 0;
	unsigned long seekoff = atoi(argv[3]);

	if(!seekoff){
		cerr<<"seek offset can not be zero"<<endl;
		exit(1);
	}

	unsigned long period;

	readCount = 0;
	period = clock();

	/// loop to iterate whole file
	while(1){
		cb.tryfill(fin);
		gcount = cb.seekoff(seekoff);
		if(!gcount) break;
		readCount += gcount;
	}
	
	/// speed of iterate given file and read count
	period = clock() - period;
	cout<<seekoff<<"\t"<<readCount<<"\t"<<period/(double)CLOCKS_PER_SEC<<endl;
	fin.close();

}
