#include "record.h"

#ifdef TEST_RECORD

#include <iostream>

int main()
{
	using namespace std;
	record r, r2;
	
	cout<<"sizeof absField: "<<sizeof(absField)<<endl;
	cout<<"sizeof field<int>: "<<sizeof(field<int>)<<endl;
	cout<<"sizeof field<char>: "<<sizeof(field<char>)<<endl;
	cout<<"sizeof field<Vector<int>>: "<<sizeof(field<Vector<int> >)<<endl;
	cout<<"sizeof Vector<int>: "<<sizeof(Vector<int>)<<endl;
	cout<<"sizeof std::vector<int>: "<<sizeof(std::vector<int>)<<endl;
	cout<<"sizeof record: "<<sizeof(record)<<endl;
	cout<<"sizeof Loki::AssocVector<std::string, absField*>"<<sizeof(Loki::AssocVector<std::string, absField*>)<<endl;
	
	r.get<int>("t") = 123;
	r.fromString("t", "345", 3);
	cout<<r.get<int>("t")<<endl;
	
	r.set<int>("t", 356);
	cout<<r.get<int>("t")<<endl;

	Vector<char> value("abc");

	Vector<char> &v(r.get<Vector<char> >("v"));
	v = value;
	//r.getv<char>("v").push_back('a');
	cout<<r.get<Vector<char> >("v")<<endl;

	cout<<r.compare("t", r2)<<endl;
	cout<<r2.compare("t", r)<<endl;
	
	view vi;
	vi.add("t");
	vi.source(r);
	cout<<vi;
	return 0;
}

#endif

