#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include "container.h"

int main()
{
	using namespace std;
	istringstream sin("cdefg");
	circular_buffer<char> cb(7);



	// 1st pass
	cb.tryfill(sin);
	
	cout<<cb.end() - cb.begin()<<endl;
	circular_buffer<char>::iterator iter1 = cb.begin();
	cout<<string(cb.begin(), cb.end())<<endl;

	iter1 = cb.end() - 1;
	do{
		cout<<*iter1;
		iter1;
	}while(iter1-- != cb.begin());
	cout<<endl;
	cout<<cb<<endl;
	cout<<make_pair(cb.begin(), cb.end())<<endl;
	cb.seekoff(2);

	// 2nd pass
	sin.clear();
	sin.str("hi");
	cb.tryfill(sin);

	cout<<cb.end() - cb.begin()<<endl;
	circular_buffer<char>::iterator iter2 = cb.begin();
	cout<<string(cb.begin(), cb.end())<<endl;
	
	iter2 = cb.end() - 1;
	do{
		cout<<*iter2;
		iter2;
	}while(iter2-- != cb.begin());
	cout<<endl;
	cout<<cb<<endl;
	cout<<make_pair(cb.begin(), cb.end())<<endl;
	cb.seekoff(2);

	// 3rd pass
	sin.clear();
	sin.str("jk");
	cb.tryfill(sin);

	cout<<cb.end() - cb.begin()<<endl;
	circular_buffer<char>::iterator iter3 = cb.begin();
	cout<<string(cb.begin(), cb.end())<<endl;

	iter3 = cb.end() - 1;
	do{
		cout<<*iter3;
		iter3;
	}while(iter3-- != cb.begin());
	cout<<endl;
	cout<<cb<<endl;
	cout<<make_pair(cb.begin(), cb.end())<<endl;
	cb.seekoff(2);

	// 4th pass
	sin.clear();
	sin.str("lm");
	cb.tryfill(sin);

	cout<<cb.end() - cb.begin()<<endl;
	circular_buffer<char>::iterator iter4 = cb.begin();
	cout<<string(cb.begin(), cb.end())<<endl;

	iter4 = cb.end() - 1;
	do{
		cout<<*iter4;
		iter4;
	}while(iter4-- != cb.begin());
	cout<<endl;
	cout<<cb<<endl;
	cout<<make_pair(cb.begin(), cb.end())<<endl;
	return 0;
}
