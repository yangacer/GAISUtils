/*! 	\file lexUtils.h
  	\brief Lexical Utilities
*/

#ifndef _LEXUTILS_H
#define _LEXUTILS_H
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>

#include "dll.h"

struct DLL_PUBLIC coder{

	std::string encode(std::string const &str);
	std::string decode(std::string const &str);
};


/**
 * \brief Generate random string
 * @param length Length of returned string
 * @return Random string
 */
std::string DLL_PUBLIC random_string(int length);

/**
 * Explode string according to delimeter (sequence)
 * @param res Container for placing results
 * @param beg Begin of string to be exploded
 * @param end End of string to be exploded
 * @param dbeg Begin of delimeter string
 * @param dend End of delimeter string
 */
template<class ResultCont, class StrIter, class DelimIter>
void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend);

/**
 * Convert explode results by given convter callback functor
 * @param res Container for placing results
 * @param beg Begin of string to be exploded
 * @param end End of string to be exploded
 * @param dbeg Begin of delimeter string
 * @param dend End of delimeter string
 * @param conv Convert functor for converting result strings
 */
template<class ResultCont, class StrIter, class DelimIter, class Convert>
void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend, Convert* conv);

/**
 * Wrapper for generic version
 * @param res Container for placing results
 * @param str C-string to be exploded
 * @param delim C-string delimeter
 * @see 
 * template<class ResultCont, class StrIter, class DelimIter>
 * void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend); 
 */
template<class ResultCont> 
inline void str_explode(ResultCont& res, char const* str, char const* delim);

/**
 * Wrapper for generic version
 * @param res Container for placing results
 * @param str C-string to be exploded
 * @param delim C-string delimeter
 * @param conv Convert functor for converting result strings
 * @see 
 * template<class ResultCont, class StrIter, class DelimIter, class Convert>
 * void str_explode(ResultCont& res, StrIter beg, StrIter end, DelimIter dbeg, DelimIter dend, Convert* conv); 
 */
template<class ResultCont, class Convert> 
inline void str_explode(ResultCont& res, char const* str, char const* delim, Convert* conv);



/**
 * Improved atoi that does not depend on '\\0' as the endian.
 * @param beg Begin of characters.
 * @param end End of characters.
 * @return Parsed value or 0 if failed.
 * @remark One can check whether the last conversion is success through the success bool member
 */
struct DLL_PUBLIC ATOI_
{
	ATOI_():success(false){}

	///  Convert leterial sequence to an integer
	template<class Iterator>
		inline int operator ()(Iterator beg, Iterator end);

	/// Convert parsed number to specific type
	template<class ReturnT, class Iterator>
		inline ReturnT conv(Iterator beg, Iterator end);
	
	template<class ReturnT>
		inline ReturnT conv(char const* str);

	bool success;
};

/// Global ATOI functor
DLL_PUBLIC extern struct ATOI_ ATOI;

/** 
 * Find pattern bruteforcely within given range.
 * @param text Begin of text.
 * @param textEnd The next pointer after the last character.
 * @param pat Begin of pattern.
 * @param patEnd The next pointer after the last character.
 * @return \ninteger >  0 - offset (from text) of the first charater of the pattern.\n iterger = -1 - not found
 */

template <class Iter1, class Iter2>
int find(Iter1 text, Iter1 textEnd, Iter2 pat, Iter2 patEnd)
{
	using namespace std;
	//if(textEnd - text < patEnd - pat) return -1;
	Iter1   iter_t = text;
	Iter2	iter_p = pat;
	while( iter_t < textEnd ){
		if( *(iter_t++) != *(pat) || textEnd - iter_t < patEnd - pat -1) continue; // skip early
		++iter_p;
#if (defined WIN32 || defined WIN32_) && defined _HAS_ITERATOR_DEBUGGING
		while(iter_p != patEnd && iter_t != textEnd && *iter_t == *iter_p){
#else
		while(*iter_t == *iter_p && iter_p != patEnd && iter_t != textEnd){
#endif
			++iter_p;
			++iter_t;
		}
		if( iter_p == patEnd){
			return iter_t - text - (patEnd - pat);
		}
		iter_t -= iter_p - pat -1;
		iter_p = pat;
	}
	return -1;

}


/** 
 * Find pattern bruteforcely within given range.
 * @param text Begin of text.
 * @param textEnd The next pointer after the last character.
 * @param pat Begin of pattern.
 * @param patEnd The next pointer after the last character.
 * @return \ninteger >  0 - offset (from text) of the first charater of the pattern.\n iterger = -1 - not found
 */
/*
template <class Iter1, class Iter2>
int find(Iter1 text, Iter1 textEnd, Iter2 pat, Iter2 patEnd)
{
	using namespace std;
	//if(textEnd - text < patEnd - pat) return -1;
	Iter1   iter_t = text;
	Iter2	iter_p = pat;
	int psize = patEnd - pat;
	int j =0;

	LOOP:	while( *iter_t != *pat && textEnd - iter_t >= psize  ) 
			++iter_t; // skip loop
		for(j=0; j < psize ; j++){
			if(*(iter_t+j) != *(iter_p+j))
				break;
		}

		if( j == psize){
			return iter_t - text;
		}

		if(textEnd - iter_t < psize)
			goto EXIT;
		++iter_t;
		goto LOOP;
	EXIT:
	return -1;

}
*/

/// Escape and un-escape text
template<typename Elem, typename Tr = std::char_traits<Elem> >
struct escaper
{
public:
	typedef typename Tr::off_type off_type;
	typedef std::basic_string<Elem, Tr> string_type;


	escaper(std::vector<string_type>& words)
		: words_(words)
	{}


	void escape(string_type& dest, const string_type& src)
	{
		dest = src;
		int pos;
		typename string_type::iterator found;

		for(int i=0; i<words_.size(); i++){
			pos = 0;
			found = dest.begin();
			while(1){
				pos = find(found, dest.end(), words_[i].begin(), words_[i].end() );
				if(pos <0) break;
				found += pos;
				dest.insert(found+1, '\\');
				found += words_[i].length() + 1;
			}
		}
		
	}

	void escape(string_type& inPlace)
	{
		escape(inPlace, inPlace);
	}

	void unescape(string_type& dest, const string_type& src)
	{
		dest = src;
		int pos;
		typename string_type::iterator found;
		string_type escapedPat;

		for(int i=0; i<words_.size(); i++){
			pos =0;
			found = dest.begin();
			escapedPat = words_[i];
			escapedPat.insert( escapedPat.begin() +1, '\\');
			while(1){
				pos = find(found, dest.end(), escapedPat.begin(), escapedPat.end());
				if(pos<0) break;
				found += pos;
				dest.erase(found+1);
				found += words_[i].length();	
			}
		}
	}

	void unescape(string_type& inPlace){
		unescape(inPlace, inPlace);
	}

private:
	std::set<int> lens_;
	std::vector<string_type> words_;

};

/// @var typedef escaper<char> to esc
typedef escaper<char> esc;

#include "lexUtils.inl"

#endif

