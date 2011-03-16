#include <iostream>
#define WIN32
#include "GAISUtils/lexUtils.h"

int main(int argc, char** argv)
{
	using namespace std;
	
	if(argc < 2){
		cout<<"atoi number_string"<<endl;
		return 0;
	}

	cout<<ATOI(&(argv[1][0]), argv[1]+strlen(argv[1]) )<<endl;
	cout<<"failed? "<<(bool)(ATOI.success == false)<<endl;
}
