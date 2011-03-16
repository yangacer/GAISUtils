#include "fileUtils.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	int size = atoi(argv[1]);
	vector<string> result;
	spliter sp(0);
	sp(result, size, argv[2]);
	for(int i=0; i<result.size(); i++){
		cout<<result[i]<<endl;		
	}
	return 0;
}
