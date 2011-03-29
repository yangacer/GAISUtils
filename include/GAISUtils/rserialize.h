#ifndef _RSERIALIZE_H
#define _RSERIALIZE_H

#include "record.h"

inline void
fromGAISRecord(record& r, char const* raw, unsigned int rsize)
{

	char const* field_name;
	std::pair<size_t, size_t> begOff;

	for(unsigned int i=0; i< (unsigned int)r.schema_->field_count();++i){
		field_name = r.schema_->find(i);
		begOff = find_field(raw, rsize, field_name, "\n@");
		if(begOff.second)
			r.fromString(field_name, raw + begOff.first, begOff.second);	
		
	}
}

#endif
