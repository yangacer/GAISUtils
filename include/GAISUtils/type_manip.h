#ifndef _TYPE_MANIP_H
#define _TYPE_MANIP_H

#include "loki/Typelist.h"


typedef Loki::TL::MakeTypelist<unsigned char, unsigned short, unsigned long, unsigned long long>::Result UnsignedTypes;

template <class T, unsigned int Size>
struct size_equal
{
	enum { value = Size == sizeof(T) };
};

template<class Head, class Tail, unsigned int s>
struct size_equal< Loki::Typelist<Head, Tail>, s>
{
	enum { value = s == sizeof(Head) };
};

namespace {

	template <class TList, unsigned int Size, bool Equal> struct FindBySize;

	template <class Head, class Tail, unsigned int Size>
		struct FindBySize< Loki::Typelist<Head, Tail>, Size, true>
		{
			typedef Head Result;
		};

	template<class Head, class Tail, unsigned int s, bool e>
		struct FindBySize< Loki::Typelist<Head, Tail>, s, e >
		{
			typedef typename FindBySize< Tail, s, size_equal<Tail, s>::value >::Result Result;
		};

}

template<class TList, unsigned int Size>
struct type_with_eq_size
{
	typedef typename FindBySize< TList, Size, size_equal<TList, Size>::value >::Result Result;
};

template<class T,class V>
struct IsPairValue;

template <class T>
struct IsPairValue<T, typename T::value_type>
{
	        enum { value = false };
};

template <class T>
struct IsPairValue<T, typename T::value_type::second_type>
{
	        enum { value = true };
};


#endif

/** Example:\n
int main()
{
	using namespace std;\n\n
	
	typedef TL::MakeTypelist<unsigned char, unsigned short, unsigned int, unsigned long long>::Result tlist;\n
	typedef FindBySize_Helper<tlist, int>::Result Result;\n
	
	cout<<sizeof(Result)<<endl;\n\n

}
*/
