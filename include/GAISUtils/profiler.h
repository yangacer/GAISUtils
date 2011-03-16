#ifndef _PROFILER_H
#define _PROFILER_H

#include <map>
#include <string>
#include "dll.h"

#ifndef WIN32
#include <sys/time.h>
#define SUBSECPREC 1000000
typedef timeval tv_t;
#else
#include <sys/types.h>
#include <sys/timeb.h>
#define SUBSECPREC 1000
typedef _timeb tv_t;
#endif



struct DLL_PUBLIC profiler
{
	struct record
	{
		tv_t beg;
		tv_t period;
	};

	void begin(char const* name);
	void end(char const* name);
	void dump(char const* name);

private:
	typedef std::map<std::string, record> Cont;
	typedef Cont::iterator tIter;
	Cont table_;
};


DLL_PUBLIC extern profiler Profiler;

#endif // end of _PROFILER_H
