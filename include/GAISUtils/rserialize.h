#ifndef _RSERIALIZE_H
#define _RSERIALIZE_H

#include "record.h"

inline void
fromGAISRecord(record &r, char const* raw, unsigned int rsize)
{

	char const* field_name;
	std::pair<size_t, size_t> begOff;
	/*
	for(unsigned int i=0; i< (unsigned int)r.schema_->field_count();++i){
		field_name = r.schema_->find(i);
		begOff = find_field(raw, rsize, field_name, "\n@");
		if(begOff.second)
			r.fromString(field_name, raw + begOff.first, begOff.second);	
		
	}
	*/
	
	rschema::iterator fIter = r.schema_->begin();
	while(fIter != r.schema_->end()){
		begOff = find_field(raw, rsize, fIter->first.c_str(), "\n@");
		if(begOff.second)
			r.fromString(fIter->first.c_str(), 
				raw + begOff.first, begOff.second);	
		++fIter;
	}
}

inline std::ostream&
toGAISRecord(record const &r, std::ostream& os)
{
	char const* field_name;
	std::pair<size_t, size_t> begOff;
	
	/*
	for(unsigned int i=0; i< (unsigned int)r.schema_->field_count();++i){
		field_name = r.schema_->find(i);
		if(field_name[0] && field_name[1] && field_name[0] == field_name[0] && field_name[0] == '_')
			continue;
		os<<field_name;
		r.writeTo(os, field_name);
		os<<"\n";
	}
	*/
	rschema::iterator fIter = r.schema_->begin();
	while(fIter != r.schema_->end()){
		if(fIter->first[0] && fIter->first[1] && 
			fIter->first[0] == fIter->first[0] && 
			fIter->first[0] == '_' ){
			++fIter;
			continue;
		}
		os<<fIter->first;
		r.vals_[fIter->second]->writeTo(os);
		++fIter;	
	}

	return os;
}

#endif
