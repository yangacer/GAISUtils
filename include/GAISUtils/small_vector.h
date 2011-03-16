#ifndef _SMALL_VECTOR_H
#define _SMALL_VECTOR_H

#include <vector>
#include <string>

template<class T>
class Vector : public std::vector
<	T, 
	smallobj_allocator<T>
>
{
	typedef std::vector<T, smallobj_allocator<T> > super;
public:
	Vector()
	: super()
	{}
	
	Vector(Vector const &cp)
	: super(cp)
	{}

	Vector(typename super::size_type s) 
	: super(s) 
	{}

	Vector(typename super::size_type s, typename super::value_type const & val)
	: super(s, val)
	{}

};

#define DEF_STR(CHAR, STRTYPE) typedef std::basic_string<CHAR, \
		std::char_traits<CHAR>, \
		smallobj_allocator<CHAR> > STRTYPE;

DEF_STR(char, String)
DEF_STR(wchar_t, WString)
DEF_STR(unsigned char, UString)
DEF_STR(unsigned wchar_t, UWString)

#endif
