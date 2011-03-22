#include "GAISUtils/record.h"

int main()
{
	try{
		init_field_factory();
		rschema schema;
		record r;


		schema.define_field("@U:", "STR");
		schema.define_field("@s:", "INT");

		schema.make(r);

		r.get<std::string>("@U:") = "abd";
		r.get<int>("@s:") = 1234;
		r.get<int>("sldkfjslkdf");

		printf(r.get<std::string>("@U:").c_str());
	}catch(char const* msg){
		printf(msg);

	}
}
