#include <iostream>
#include <fstream>
#include "lexUtils.h"
#include "container.h"

int main(int argc, char** argv)
{
	using namespace std;

	//istringstream sin("test data with aceace pattern and acer pattern");
	ifstream fin(argv[1]);

	char const * pat = "acer";

	circular_buffer<char> cb(10);
	unsigned int readCount = 0, prevRead;
	int foundAt;
	while(1){
		cb.tryfill(fin);
		foundAt = find(cb.begin(), cb.end(), pat, pat+4);
		if( -1 != foundAt ){
			for(int i=0; i<4;i++){
				cout<<*(cb.begin()+foundAt+i);
			}
			cout<<" at "<<readCount+foundAt<<endl;
			prevRead = cb.seekoff(foundAt+4);
		}else if(cb.size() > 4){
			prevRead =  cb.seekoff(cb.size() - 3);
		}else{
			prevRead = cb.seekoff(10);
		}
		if(!prevRead) break;
		readCount += prevRead;

	}
	cout<<readCount<<endl;
	return 0;
}
