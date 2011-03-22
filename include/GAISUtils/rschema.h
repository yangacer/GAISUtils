#ifndef _RSCHEMA_H
#define _RSCHEMA_H

#include <string>
#include "loki/AssocVector.h"

typedef unsigned char FIELD_INDEX;

struct record;

struct rschema 
{
	typedef Loki::AssocVector<std::string, FIELD_INDEX> Index;
	
	rschema();

	rschema&
	define_field(char const* field_name, char const* type_str);
	
	FIELD_INDEX
	find(char const* field_name);

	void
	make(record& r);

private:
	Index idx_;
	record *proto_;
};

#endif
