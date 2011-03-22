#include "rschema.h"
#include "record.h"

rschema::rschema()
: proto_(new record)
{
	proto_->schema_ = this;	
}

rschema&
rschema::define_field(char const* field_name, char const* type_str)
{
	try{
		proto_->vals_.push_back(
			field_factory::Instance().CreateObject(type_str));
		idx_[field_name] = proto_->vals_.size() - 1;
	}catch(...){
		throw "rschema: defin_field failed";	
	}

}

FIELD_INDEX
rschema::find(char const* field_name)
{
	return idx_[field_name];	
}

void
rschema::make(record& r)
{	r = *proto_;	}
