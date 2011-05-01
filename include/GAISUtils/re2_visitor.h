#ifndef _RE2_VISITOR_H
#define _RE2_VISITOR_H

#include "record.h"
#include "re2/re2.h"

namespace RE2_BIND
{
	using namespace re2;
	
	typedef Loki::Visitor<field<std::string>, void, true> StrVisitor;	

	struct FullMatch : 
	public Loki::BaseVisitor, 
	public StrVisitor
	{
		FullMatch(char const *pat) 
		: pattern(pat)
		{}

		void
		Visit(field<std::string> const &f)
		{ rt = RE2::FullMatch(f.val_, pattern); }
		
		bool operator()(absField *f)
		{ f->Accept(*this); return rt; }

		bool rt;
		RE2 pattern;
	};

	struct PartialMatch : 
	public Loki::BaseVisitor, 
	public StrVisitor
	{
		PartialMatch(char const *pat)
		: pattern(pat)
		{}

		void
		Visit(field<std::string> const &f)
		{ rt = RE2::PartialMatch(f.val_, pattern); }
		
		bool operator()(absField *f)
		{ f->Accept(*this); return rt; }

		bool rt;
		RE2 pattern;
	};

	struct FindAndConsume : 
	public Loki::BaseVisitor, 
	public StrVisitor
	{

		FindAndConsume(char const *pat, absField* af)
		: pattern(pat)
		{	af->Accept(*this);	}

		void		
		Visit(field<std::string> const &f)
		{ sp.set(f.val_.c_str(), f.val_.size()); }


		bool operator()(std::string *s1)
		{ return RE2::FindAndConsume(&sp, pattern, s1); }

		bool operator()(std::string *s1, std::string *s2)
		{ return RE2::FindAndConsume(&sp, pattern, s1, s2); }

		bool operator()(std::string *s1, std::string *s2, std::string *s3)
		{ return RE2::FindAndConsume(&sp, pattern, s1, s2, s3); }

		bool operator()(std::string *s1, std::string *s2, std::string *s3, std::string *s4)
		{ return RE2::FindAndConsume(&sp, pattern, s1, s2, s3, s4); }


		StringPiece sp;
		RE2 pattern;
	};
} // end of RE2_BIND namespace


#endif 
