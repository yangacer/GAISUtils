#include <iostream>
#include <vector>
#include <string>
#include "../lexUtils.h"

int main(int argc, char** argv)
{
	using namespace std;

	const char* delim = ", ";
	string str = "a, b , c, d, ";
	string ints = "1, 123, 234, 99";

	vector<string> res;
	vector<int> res_int;

	str_explode(res, str.begin(), str.end(), delim, delim+2);
	str_explode(res_int, ints.begin(), ints.end(), delim, delim+2, &ATOI);

	for(int i=0; i<res.size(); i++){
		cout<<res[i]<<endl;
	}

	for(int i=0; i<res_int.size(); i++){
		cout<<res_int[i]<<endl;
	}
	return 0;
}
