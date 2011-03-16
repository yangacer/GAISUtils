#include <ctime>
#include "timeUtils.h"

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

int time_zone_distance()
{
	time_t curr=time(0);			// current local time
	tm local=*gmtime(&curr);		// convert curr to GMT, store as tm
	time_t utc = (mktime(&local));		// convert GMT tm to GMT time_t
	double diff=difftime(curr, utc)/3600; 	//difference in hours
	return (int)diff;
}


