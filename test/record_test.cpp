#include "GAISUtils/record.h"
#include <iostream>
#include <string>

int main()
{
	using namespace std;

	try{
		// *** MUST init factory before 
		// any record are declared.
		init_field_factory();

		rschema schema;
		record r;

		// setup schema
		schema.define_field("@U:", "STR");
		schema.define_field("@s:", "INT");
		
		// let record r use the schema just 
		schema.make(r);

		// set value when you know the types
		r.get<std::string>("@U:") = "abd";
		r.get<int>("@s:") = 1234;
		
		r.set<std::string>("@U:", "abd"); // equalevent
		r.set<int>("@s:", 1234);
		
		// output
		cout<<r.toString("@U:")<<endl;
		cout<<r.toString("@s:")<<endl;
		
		// set value when type is unknown
		r.fromString("@U:", "def");
		r.fromString("@s:", "4567");
		
		// output via write to ostream directly
		r.writeTo(cout, "@U:");
		r.writeTo(cout, "@s:");
		
		// compare methods
		bool cmpRt;
		cout<<"compare @s field with value 123"<<endl;
		cmpRt = r.compare<int>("@s:", 123) > 0; // cmpRt == true
		cout<<"compare result: "<<cmpRt<<endl;
		
		cout<<"compare @s field with another record (itself actually).";
		cmpRt = r.compare("@s:", r) == 0; // cmpRt == true
		cout<<"compare result: "<<cmpRt<<endl;

	}catch(char const* msg){
		printf(msg);

	}
}
