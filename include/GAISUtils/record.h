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

#include <cstring>
#include <sstream>
#include <functional>
#include <string>
#include <vector>

class absField
{
public:

	virtual absField* Clone() const = 0;
	virtual ~absField(){}
	virtual int compare(absField const* rhs, bool) const = 0;
	virtual int compare(absField const* rhs, void* compare, bool) const = 0;
	virtual bool fromString(char const *cstr, size_t size) = 0;
	virtual bool fromString(char const *str) = 0;
	virtual std::string toString() const = 0;
	virtual std::ostream &writeTo(std::ostream &os) const = 0;
};

class record;

template<class T>
struct create_field;

template<typename T>
class field : public absField, public Loki::SmallObject<>
{

	typedef int(*CompareFunc)(T const&, T const&);
public:

	int 
	compare(absField const* rhs, bool sameType = false) const 
	throw(char const*)
	{
		field* p = (sameType) ? 
			static_cast<field*>(const_cast<absField*>(rhs))
			: dynamic_cast<field*>(	const_cast<absField*>(rhs));

		if(0 == p)
			return -2;
		if(val_ < p->val_)
			return -1;
		if(val_ > p->val_)
			return 1;
		return 0;
	}
	
	int
	compare(absField const *rhs, void* cmp, bool sameType = false) const
	throw(char const*)
	{
		field* p = (sameType) ? 
			static_cast<field*>(const_cast<absField*>(rhs))
			: dynamic_cast<field*>(	const_cast<absField*>(rhs));

		return ((CompareFunc)cmp)(this->val_, p->val_);
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
	
	std::ostream&
	writeTo(std::ostream &os) const
	{ os<<val_; return os;}

private: // Client (out of GAISUtils lib) never creates field object directly
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

	
	T val_;
};

/// Field Factory
typedef Loki::SingletonHolder
	<
		Loki::Factory< absField, std::string >,
		Loki::CreateUsingNew,
		Loki::LongevityLifetime::DieAsSmallObjectChild
	> field_factory;



/// Multiple Typed Fields Record
class record
{
	friend class rschema;
	friend void fromGAISRecord(record& r, char const* raw, unsigned int rsize);
	friend std::ostream& toGAISRecord(record const& r, std::ostream& os);
protected:
	typedef std::vector<absField*> StorageType;
public:

	record()
	: schema_(0)
	{}
	
	record(record const &cp)
	: schema_(cp.schema_), schema_ver_(cp.schema_ver_)
	{
		if(this != &cp){
			StorageType::iterator iter(cp.vals_.begin());
			while(iter != cp.vals_.end()){
				vals_.push_back((*iter)->Clone());
				++iter;
			}
		}
	}
	
	record& 
	operator=(record const &cp)
	{
		if(this == &cp)
			return *this;
	
		StorageType::iterator iter(vals_.begin()), myIter;
		if(!isSameSchema(cp)){
			while(iter != vals_.end()){
				delete (*iter);
				++iter;
			}
			vals_.clear();

			schema_ = cp.schema_;
			schema_ver_ = cp.schema_ver_;
			iter = cp.vals_.begin();
			while(iter != cp.vals_.end()){
				vals_.push_back((*iter)->Clone());
				++iter;
			}
		}
		myIter = vals_.begin();
		iter = cp.vals_.begin();
		while(iter != cp.vals_.end()){
			*myIter = (*iter)->Clone();
			++iter;
		}
		return *this;
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

	/** Get reference of field value
	 *  @tparam T Type of the field value
	 *  @param field_name
	 *  @return Reference of value of field
	 *  @remark Throw exception when T was not matched.
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
	
	/** Get constant reference of field value
	 *  @tparam T Type of the field value
	 *  @param field_name
	 *  @return Constant reference of value of field
	 *  @remark Throw exception when T was not matched.
	 */
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
	
	/** Set value according to given field name and value of type T.
	 *  @tparam T Type of the field value
	 *  @param field_value
	 *  @param val Value of type T
	 *  @remark Throw exception (via get<T>(field_name)) when T was not matched
	 */
	template<typename T>
	void 
	set(char const* field_name, T const & val)
	{
		get<T>(field_name) = val;	
	}

	/** Set field value converted from text.
	 *  @param field_name
	 *  @param cstr
	 *  @param size
	 *  @return True for conversion successed. False for failure.
	 */
	bool 
	fromString(char const* field_name, char const *cstr, size_t size)
	{
		return vals_[schema_->find(field_name)]->fromString(cstr, size);
	}

	
	/** C-string version of fromString(char const*, char const*, size_t)
	 */
	bool 
	fromString(char const* field_name, char const* str)
	{
		return vals_[schema_->find(field_name)]->fromString(str);	
	}

	/** toString methods are provided for
	 * convinentness, they cost extra allocation
	 * for a local stringstream.
	 * To use a external stringsteram and access
	 * field via record::get<T> is recommanded.
	 */
	std::string 
	toString(char const* field_name) const
	{
		return vals_[schema_->find(field_name)]->toString();	
	}
	
	/** Write field value in text format to given output stream
	 *  @param os
	 *  @param field_name
	 *  @return os
	 */
	std::ostream&
	writeTo(std::ostream& os, char const* field_name) const
	{	return vals_[schema_->find(field_name)]->writeTo(os);  }

	/** Compare specified fields of two records.
	 * @param field_name
	 * @param rhs compared record
	 * @return As follows:<br/>
	 * @code
	 *  0 - Equal
	 *  1 - larger
	 * -1 - less
	 * -2 - uncomparable
	 * @endcode
	 */	
	int 
	compare(char const *field_name, record const & rhs) const
	{
		return vals_[schema_->find(field_name)]->
			compare( rhs.vals_[rhs.schema_->find(field_name)], isSameSchema(rhs) );
	}
	
	int
	compare(char const *field_name, record const& rhs, void* compare)
	{
		return vals_[schema_->find(field_name)]->
			compare( rhs.vals_[rhs.schema_->find(field_name)], compare, isSameSchema(rhs) );
	}

	/** Compare a field with value of type given
	 * @tparam T Type of field value
	 * @param field_name
	 * @param rhs Value of type T
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
	
	template<typename T>
	int
	compare(char const* field_name, T const &rhs, void* compare) const
	{
		try{
			T const &lhs = get<T>(field_name);
			return ((typename field<T>::CompareFunc)compare)(lhs, rhs);
		}catch(...){
			return 	-2;
		}
		return -2;
	}

	typedef StorageType::iterator iterator;
	typedef StorageType::const_iterator const_iterator;
	
	/** Get begin iterator of fields
	 *  @return iterator
	 */
	StorageType::iterator
	begin()
	{ return vals_.begin(); }

	/** Get end iterator of fields
	 *  @return iterator
	 */
	StorageType::iterator
	end()
	{ return vals_.end();	}

	/** Get constant begin iterator
	 */
	StorageType::const_iterator
	const_begin() const
	{ return vals_.begin(); }

	/** Get constant end iterator
	 */
	StorageType::const_iterator
	const_end() const
	{ return vals_.end();	}
	
	rschema
	schema() const
	{ return rschema(*schema_); } 

	bool
	isSameSchema(record const &r) const
	{ 
		return schema_ == r.schema_ && 
			schema_ver_ == r.schema_ver_; 
	}

	absField const*
	operator[](FIELD_INDEX i) const
	{ return vals_[i]; }

private:
	// fields layout can be fixed
	// different record fields layouts can be centrialized
	// to a single(ton) manager
 	mutable StorageType vals_;
	rschema *schema_;
	unsigned int schema_ver_;
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
	using std::strlen;

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

