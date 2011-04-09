#include "GAISUtils/record.h"
#include <iostream>

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
		
		// output
		cout<<r.toString("@U:")<<endl;
		cout<<r.toString("@s:")<<endl;
		
		// set value when type is unknown
		r.fromString("@U:", "def");
		r.fromString("@s:", "4567");
		
		// output via write to ostream directly
		r.writeTo(cout, "@U:");
		r.writeTo(cout, "@s:");

	}catch(char const* msg){
		printf(msg);

	}
}
