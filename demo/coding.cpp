#include "lexUtils.h"
#include <iostream>

int main(int argc, char** argv)
{
	using namespace std;

	if(argc < 2) return 0;
	cout<<"Original string: "<<argv[1]<<endl;
	coder cr;
	cout<<"Encode then decode: "<<cr.decode(cr.encode(string(argv[1])))<<endl;
	return 0;
}
