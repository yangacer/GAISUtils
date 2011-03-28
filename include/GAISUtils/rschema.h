#ifndef _RSCHEMA_H
#define _RSCHEMA_H

#include <string>
#include "loki/AssocVector.h"

typedef unsigned char FIELD_INDEX;

struct record;

struct rschema 
{	
	template <class T, class U>
	struct IndexCont
	{
		typedef Loki::AssocVector<T,U> Type;
	};

	typedef IndexCont<std::string, FIELD_INDEX>::Type Index;
	typedef IndexCont<FIELD_INDEX, std::string>::Type InvertIndex;

	rschema();

	rschema&
	define_field(char const* field_name, char const* type_str);
	
	FIELD_INDEX
	find(char const* field_name);
	
	char const*
	find(FIELD_INDEX field_index);

	void
	make(record& r);

private:
	Index idx_;
	InvertIndex ridx_;	
	record *proto_;
};

#endif
