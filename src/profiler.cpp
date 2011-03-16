
#include <iostream>
#include <iomanip>
#include "profiler.h"

#ifndef WIN32
#define SUBSEC_DIGIT 6
#define SEC(X) X.tv_sec
#define SUBSEC(X) X.tv_usec
#define GETTIME(X) gettimeofday(X, 0)
#else
#define SUBSEC_DIGIT 3
#define SEC(X) x.time
#define SUBSEC(X) X.millitm
#define GETTIME(X) _ftime_s(X);
#endif

profiler Profiler;

tv_t tv_off(tv_t beg, tv_t end)
{
	tv_t rt;
	SEC(rt) = SEC(end) - SEC(beg);
	if(SUBSEC(end) < SUBSEC(beg)){
		SUBSEC(rt) = SUBSEC(end) + SUBSECPREC - SUBSEC(beg);
		SEC(rt)--;
	}else
		SUBSEC(rt) = SUBSEC(end) - SUBSEC(beg);
	return rt;
}

tv_t tv_add(tv_t x, tv_t y)
{
	tv_t rt;
	SUBSEC(rt) = SUBSEC(x) + SUBSEC(y);
	SEC(rt) = SEC(x) + SEC(y) + SUBSEC(rt)/SUBSECPREC;
	SUBSEC(rt) %= SUBSECPREC;
	return rt;
}

double tv_fraction(tv_t x, tv_t y)
{
	return ((double)SEC(x) + SUBSEC(x)/(double)SUBSECPREC) / ((double)SEC(y) + SUBSEC(y) /(double)SUBSECPREC);
}


void 
profiler::begin(char const *name)
{


	record r;
	GETTIME(&r.beg);
	SEC(r.period) = 0;
	SUBSEC(r.period) = 0;

	tIter iter;
	if( (iter = table_.find(name)) == table_.end()){
		table_[name] = r;
		return;
	}else{
		iter->second.beg = r.beg;
	}
}

void 
profiler::end(char const *name)
{
	tv_t end;
	GETTIME(&end);
	
	tIter iter;
	if((iter = table_.find(name)) == table_.end())
		return;
	else{
		tv_t &beg = iter->second.beg;
		tv_t &period = iter->second.period;
		tv_t nPeriod;
		
		// diff
		nPeriod = tv_off(beg, end);
		// update period
		period = tv_add(period, nPeriod);
	}
}

void 
profiler::dump(char const *name)
{
	using namespace std;
	
	tIter iter;
	if((iter = table_.find(name)) == table_.end())
		return;
	else{
		ios::fmtflags flag = cout.flags();
		cout.unsetf(flag);
		
		tv_t &period = iter->second.period;
		for(tIter i = table_.begin(); i != table_.end(); ++i){
			if(i == iter) continue;
			tv_t &subPeriod = i->second.period;
			cout<<(i->first)<<": "<<
				SEC(subPeriod)<<"."<<
				setw(SUBSEC_DIGIT)<<setfill('0')<<SUBSEC(subPeriod)<<
				" "<<(100*tv_fraction(subPeriod, period))<<"%"
				<<endl;
		}

		cout<<name<<": "<<
			SEC(period)<<"."<<
			setw(SUBSEC_DIGIT)<<setfill('0')<<SUBSEC(period)<<
			endl;
		cout.setf(flag);
	}
}



