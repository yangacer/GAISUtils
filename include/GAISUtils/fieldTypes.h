#ifndef FIELDTYPES_H
#define FIELDTYPES_H

#include <string>
#include "loki/Sequence.h"

template<typename T>
class field;

template<class TList>
struct FieldType;

template<typename Head, typename Tail>
struct FieldType<Loki::Typelist<Head, Tail> >
{
	typedef Loki::Typelist< field<Head> , typename FieldType<Tail>::Type > Type;
};

template<>
struct FieldType<Loki::NullType>
{ typedef Loki::NullType Type; };



typedef FieldType<
Loki::Seq<
	int, unsigned int,
	char, unsigned char, std::string, 
	double, long long, unsigned long long>::Type 
>::Type SupportedTypes;


#endif
