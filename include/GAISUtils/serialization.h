#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

#include <iostream>
#include <string>
#include <iterator>
#include <utility>

namespace Serialization
{

	template<class Ostream, class Cont>
	Ostream& output(Ostream& os, Cont& cont )
	{

		typename Cont::iterator iter = cont.begin();
		while(iter != cont.end()){
			os<<*iter<<"\n";
			iter++;
		}
		//os<<"\n";
		return os;
	}

	template<class Istream, class Cont>
	Istream& input(Istream& is, Cont& cont)
	{ 
		typename Cont::value_type val;
		
		std::insert_iterator<Cont> inserter(cont, cont.begin());

		while(!is.eof()){
			is>>val;
			if(!is.fail()){
				*inserter = val;
				inserter++;
			}
		}
	}

	
	template<class Ostream, typename T, typename U>
	Ostream& operator <<(Ostream& os, std::pair<T,U>& p)
	{
		os<<p.first<<"\t"<<p.second;
		return os;
	}

	template<typename Istream, typename T, typename U>
	Istream& operator >>(Istream& is, std::pair<T,U>& p)
	{
		is>>p.first;
		is>>p.second;
		return is;
	}
}

#endif

