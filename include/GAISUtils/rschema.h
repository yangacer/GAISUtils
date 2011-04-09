#ifndef _RSCHEMA_H
#define _RSCHEMA_H

#include <string>
#include <vector>
#include <iosfwd>
#include "loki/AssocVector.h"

typedef unsigned char FIELD_INDEX;

struct record;
struct rschema;

std::ostream& operator<<(std::ostream& os, rschema &rs);
std::istream& operator>>(std::istream& is, rschema &rs);

struct rschema 
{	
	friend std::ostream& operator<<(std::ostream& os, rschema &rs);
	friend std::istream& operator>>(std::istream& is, rschema &rs);

	template <class T, class U>
	struct IndexCont
	{
		typedef Loki::AssocVector<T,U> Type;
	};

	typedef IndexCont<std::string, FIELD_INDEX>::Type Index;
	typedef IndexCont<FIELD_INDEX, std::string>::Type InvertIndex;
	
	rschema();
	
	rschema(char const *name);

	rschema&
	define_field(char const* field_name, char const* type_str);
	
	rschema&
	undefine_field(char const* field_name);

	char const*
	name() const;
	
	rschema&
	name(char const*);

	FIELD_INDEX
	find(char const* field_name) const;
	
	char const*
	find(FIELD_INDEX field_index) const;

	void
	make(record& r);
	
	unsigned int
	field_count() const;
	
	unsigned int
	version() const;
private:
	Index idx_;
	InvertIndex ridx_, types_;	
	record *proto_;
	std::string name_;
	unsigned int version_;
};

#endif
