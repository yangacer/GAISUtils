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
		ridx_[(FIELD_INDEX)proto_->vals_.size() - 1].assign(field_name);
	}catch(...){
		throw "rschema: defin_field failed";	
	}

}

rschema&
rschema::undefine_field(char const* field_name)
{
	Index::iterator iter = idx_.find(field_name);
	if(iter == idx_.end())
		return *this;
	delete proto_->vals_[iter->second];
	proto_->vals_.erase(proto_->vals_.begin() + iter->second);
	ridx_.erase(iter->second);
	idx_.erase(iter);
	return *this;
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
	return iter->second;	
}

char const*
rschema::find(FIELD_INDEX field_index)
{
	InvertIndex::iterator iter = ridx_.find(field_index);
	if(iter == ridx_.end())
		throw "rschema: non-exist field index";
	return iter->second.c_str();
}

void
rschema::make(record& r)
{	r = *proto_;	}

unsigned int
rschema::field_count() const
{ return proto_?proto_->vals_.size():0; }
