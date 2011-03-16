#include "partialOmniORB.h"

static inline omniORB::logFunction& logfunc()
{
	  static omniORB::logFunction f = 0;
	    return f;
}

void
omniORB::setLogFunction(omniORB::logFunction f)
{
	  logfunc() = f;
}

