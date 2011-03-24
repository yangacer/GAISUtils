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
		Index::iterator iter = idx_.find(field_name);
		if(iter != idx_.end()){
			std::string msg("rschema: redefine field ");
			msg += field_name ;
			msg += " - ";
			msg += type_str;
			throw msg.c_str();
		}
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
	Index::iterator iter = idx_.find(field_name);
	if(iter == idx_.end()){
		std::string msg("rschema: non-exist field name(");
		msg += field_name;
		msg += ")";
		throw msg.c_str();
	}
	return idx_[field_name];	
}

void
rschema::make(record& r)
{	r = *proto_;	}
