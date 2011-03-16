//#include "lexUtils.h"
#include <iterator>
#include <cstring>

template<class ResultStr, class Sequence>
void str_implode(ResultStr &res, Sequence const & sequence, typename ResultStr::value_type delim)
{
	typedef typename Sequence::const_iterator iterator;
	iterator i = sequence.begin();
	for(i; i + 1 != sequence.end(); ++i){
		res += *i + delim;
	}
	if(sequence.size())
		res += *i;
}

template<class ResultCont, class StrIter, class DelimIter>
void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend)
{
	typedef typename ResultCont::value_type value_type;
	std::insert_iterator< ResultCont > iter(res, res.end());
	
	int offset=0, pos;
	while(-1 != (pos = find(beg+offset, end, dbeg, dend)) ){
		*iter = value_type(beg + offset, beg + offset + pos);
		++iter;
		offset += pos + dend - dbeg;
	}

	if( beg + offset < end){
		*iter = value_type(beg + offset, end);
	}
}

template<class ResultCont, class StrIter, class DelimIter, class Convert>
void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend, Convert* conv)
{
	typedef typename ResultCont::value_type value_type;
	std::insert_iterator< ResultCont > iter(res, res.end());

	int offset=0, pos;
	while(-1 != (pos = find(beg+offset, end, dbeg, dend)) ){
		*iter = value_type((*conv)(beg + offset, beg + offset + pos));
		++iter;
		offset += pos + dend - dbeg;
	}

	if( beg + offset < end){
		*iter = value_type((*conv)(beg + offset, end));
	}
}


template<class ResultCont>
inline void str_explode(ResultCont& res, char const* str, char const* delim)
{
	str_explode(res, str, str+strlen(str), delim, delim+strlen(delim));
}


template<class ResultCont, class Convert> 
inline void str_explode(ResultCont& res, char const* str, char const* delim, Convert* conv)
{
	str_explode(res, str, str+strlen(str), delim, delim+strlen(delim), conv);
}

template<class Iterator>
inline int ATOI_::operator ()(Iterator beg, Iterator end)
{
	int sign = 1, result = 0;
	
	if(*beg == '-'){
		beg++;
		sign = -1;
	}

	if(*beg == '0' && (end - beg > 1 || sign < 0)){
		success = false;
		return 0;
	}

	for(Iterator iter = beg; iter != end; iter++){
		if(!std::isdigit(*iter)){
			success = false;
			return 0;
		}
		result += (*iter)-0x30;
		result *= 10;
	}
	success = true;
	result /= 10;
	return result * sign;
}

template<class ReturnT, class Iterator>
inline ReturnT ATOI_::conv(Iterator beg, Iterator end)
{
	ReturnT sign = 1, result = 0;
	
	if(*beg == '-'){
		beg++;
		sign = -1;
	}

	if(*beg == '0' && (end - beg > 1 || sign < 0)){
		success = false;
		return 0;
	}

	for(Iterator iter = beg; iter != end; iter++){
		if(!std::isdigit(*iter)){
			success = false;
			return 0;
		}
		result += (*iter)-0x30;
		result *= 10;
	}
	success = true;
	result /= 10;
	return result * sign;
}

template<class ReturnT>
inline ReturnT ATOI_::conv(char const* str)
{
	return conv<ReturnT>(str, str+strlen(str));
}
