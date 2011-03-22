#include "GAISUtils/record.h"

int main()
{
	init_field_factory();
	rschema schema;
	
	schema.define_field("@U:", "STR");
	schema.define_field("@s:", "INT");

	record r;
	schema.make(r);

	r.get<std::string>("@U:") = "abd";
	r.get<int>("@s:") = 1234;

	printf(r.get<std::string>("@U:").c_str());
}
