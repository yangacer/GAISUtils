#ifndef _RECORD_H
#define _RECORD_H

#ifndef LOKI_DEFAULT_CHUNK_SIZE
#define LOKI_DEFAULT_CHUNK_SIZE 1024*1024
#endif

#include "loki/Factory.h"
#include "loki/AssocVector.h"
#include "loki/SmallObj.h"
#include "mem_helper.h"
#include "small_vector.h"
#include "rschema.h"

#include <sstream>
#include <functional>
#include <string>
#include <vector>

class absField
{
public:
	virtual absField* Clone() const = 0;
	virtual ~absField(){}
	virtual int compare(absField const* rhs) const = 0;
	virtual bool fromString(char const *cstr, size_t size) = 0;
	virtual bool fromString(char const *str) = 0;
	virtual std::string toString() const = 0;
};

class record;

template<class T>
struct create_field;

template<typename T>
class field : public absField, public Loki::SmallObject<>
{
private: // Client never creates field object directly
	friend class record;
	template<class T1> friend struct create_field;

	typedef T value_type;

	field()
	: val_()
	{}

	field(field const &cp)
	: val_(cp.val_)
	{}

	~field()
	{}

	field* 
	Clone() const
	{ return new field(*this); }

	int 
	compare(absField const* rhs) const throw(char const*)
	{
		field* p = 
			dynamic_cast<field*>(
				const_cast<absField*>(rhs)
			);

		if(0 == p)
			return -2;
			//throw "Compare between different type";
		if(val_ < p->val_)
			return -1;
		if(val_ > p->val_)
			return 1;
		return 0;
	}
	
	bool 
	fromString(char const *cstr, size_t size)
	{
		std::stringstream cvt;
		cvt.write(cstr, size);
		cvt>>val_;
		return (bool)cvt;
	}

	bool 
	fromString(char const *str)
	{
		std::stringstream cvt(str);
		cvt>>val_;
		return (bool)cvt;
	}


	std::string 
	toString() const 
	{
		std::stringstream cvt;
		cvt<<val_;
		return cvt.str();
	}

	T val_;
};

/// Field Factory
typedef Loki::SingletonHolder
	<
		Loki::Factory< absField, std::string >,
		Loki::CreateUsingNew,
		Loki::LongevityLifetime::DieAsSmallObjectChild
	> field_factory;


void fromGAISRecord(record& r, char const* raw, unsigned int rsize);

/// Multiple Typed Fields Record
class record
{
	friend class rschema;
	friend void fromGAISRecord(record& r, char const* raw, unsigned int rsize);
protected:
	// AssocVector cost:
	// O(N) to insert/delete 
	// and O(lgN) to search
	typedef std::vector<absField*> StorageType;

public:
	
	record()
	: schema_(0)
	{}
	
	record(record const &cp)
	: schema_(cp.schema_)
	{
		StorageType::iterator iter(cp.vals_.begin());
		while(iter != cp.vals_.end()){
			vals_.push_back((*iter)->Clone());
			++iter;
		}
	}
	
	record& 
	operator=(record const &cp)
	{

		StorageType::iterator iter(vals_.begin());
		while(iter != vals_.end()){
			delete (*iter);
			++iter;
		}
		vals_.clear();

		schema_ = cp.schema_;
		
		iter = cp.vals_.begin();
		while(iter != cp.vals_.end()){
			/*
			if(vals_.find(iter->first) != vals_.end()){
				delete vals_[iter->first];	
			}
			*/
			vals_.push_back((*iter)->Clone());
			++iter;
		}	
	}
	
	~record()
	{
		StorageType::iterator iter(vals_.begin());
		while(iter != vals_.end()){
			delete (*iter);
			++iter;
		}
		vals_.clear();
	}

	/*
	record& 
	define_field(char const* field_name, char const* type_str)
	{
		try{
			vals_[field_name] = 
				field_factory::Instance().CreateObject(type_str);
		}catch(...){
			throw "record:;defin_field failed";	
		}
		return *this;	
	}
	*/

	template<typename T>
	typename field<T>::value_type& 
	get(char const* field_name) throw(char const*)
	{
		field<T>* p(dynamic_cast<field<T>*>(vals_[schema_->find(field_name)]));
		if(0 == p)
			throw "record: Try to convert field<T> to wrong type";
		return (p->val_);
	}
	

	template<typename T>
	typename field<T>::value_type const&
	get(char const* field_name) const throw(char const*)
	{
		field<T> const* p = 
			dynamic_cast<field<T> const*>(vals_[schema_->find(field_name)]);
		if(0 == p)
			throw "record: Try to convert field<T> to wrong type";
		return (p->val_);

	}
	
	template<typename T>
	void 
	set(char const* field_name, T const & val)
	{
		get<T>(field_name) = val;	
	}

	// from/to string methods are provided for
	// convinentness, they cost extra allocation
	// for a local stringstream.
	// To use a external stringsteram and access
	// field via record::get<T> is recommanded.
	bool 
	fromString(char const* field_name, char const *cstr, size_t size)
	{
		return vals_[schema_->find(field_name)]->fromString(cstr, size);
	}
	
	bool 
	fromString(char const* field_name, char const* str)
	{
		return vals_[schema_->find(field_name)]->fromString(str);	
	}

	
	std::string 
	toString(char const* field_name) const
	{
		return vals_[schema_->find(field_name)]->toString();	
	}

	int 
	compare(char const *field_name, record const & rhs) const
	{
		assert(0 != schema_);
		return vals_[schema_->find(field_name)]->compare(rhs.vals_[schema_->find(field_name)]);
	}
	
	/** Compare a field with value of type given
	 * @param field_name
	 * @param rhs Value of type T(template parameter)
	 * @return As follows:<br/>
	 * @code
	 *  0 - Equal
	 *  1 - larger
	 * -1 - less
	 * -2 - uncomparable
	 * @endcode
	 */
	template<typename T>
	int
	compare(char const* field_name, T const &rhs) const
	{
		try{
			T const &lhs = get<T>(field_name);
			if(lhs < rhs)
				return -1;
			if(lhs > rhs)
				return 1;
			return 0;
		}catch(...){
			return 	-2;
		}
		return -2;
	}

protected:
	// fields layout can be fixed
	// different record fields layouts can be centrialized
	// to a single(ton) manager
 	mutable StorageType vals_;
	rschema *schema_;
};


/// Null Creator Template
template<class T>
struct create_field
{
	absField* operator()()
	{
		return new field<T>;
	}	
};


inline void init_field_factory()
{
	//
	// Commented lines require additional serialization operators
	// that not supported by standard C++ lib.
	
	field_factory::ObjectType &factory(field_factory::Instance());

	factory.Register("INT", create_field<int>());

	factory.Register("UINT", create_field<unsigned int>());
	
	//factory.Register("WCHAR", create_field<wchar_t>());
	//factory.Register("UWCHAR", create_field<unsigned wchar_t>());

	factory.Register("CHAR", create_field<char>());
	factory.Register("UCHAR", create_field<unsigned char>());

	factory.Register("DOUBLE", create_field<double>());
	factory.Register("LLONG", create_field<long long>());
	factory.Register("ULLONG", create_field<unsigned long long>());

	// vector
	
	
	//factory.Register("INT[]", create_field<Vector<int> >());
	//factory.Register("UNIT[]", create_field<Vector<unsigned int> >());
	

	factory.Register("CHAR[]", create_field<std::string>());
	factory.Register("STR", create_field<std::string>());
	
	
	//factory.Register("UCHAR[]", create_field<Vector<unsigned char> >());
	
	//factory.Register("DOUBLE[]", create_field<Vector<double> >());
	//factory.Register("LLONG[]", create_field<Vector<long long> >());
	//factory.Register("ULLONG[]", create_field<Vector<unsigned long long> >());
}

/**
 * @param text Text to be searched
 * @param tsize Text size
 * @param begin_pattern Null terminated c-string
 * @param end_pattern Default is newline. If 0 is given, this function
 * matches the text end as a field end.
 * @param hint_offset Offset for start to search begin_pattern.
 * @return (offset, size) pair. When field is not found, 
 * the offset will be set to size_t(-1) and size will be zero
 */
inline std::pair<size_t, size_t> find_field(
	char const *text,
	size_t tsize,
	char const *begin_pattern, 
	char const *end_pattern = "\n", 
	size_t hint_offset = 0)
{
	std::pair<size_t, size_t> rt((size_t)-1, 0);
	char const *bp_end(begin_pattern + strlen(begin_pattern));
	char const *txt_end(text + tsize);
	
	if(text + hint_offset >= text + tsize)
		return rt;

	char const *bp_pos = std::search(
		text + hint_offset, txt_end, 
		begin_pattern, bp_end
	);


	// Not found
	if(bp_pos == txt_end)
		return rt;
	

	bp_pos += bp_end - begin_pattern;
	rt.first = bp_pos - text;

	bp_pos = std::search(
		bp_pos, txt_end, end_pattern, 
		end_pattern + strlen(end_pattern)
	);
	
	// Not found
	if(bp_pos == txt_end){
		rt.first = (size_t)-1;
		return rt;
	}

	rt.second = bp_pos - (text + rt.first);
	return rt;
}


#endif

