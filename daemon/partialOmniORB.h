#ifndef _PARTIALOMNIORB_H
#define _PARTIALOMNIORB_H
namespace omniORB{
	typedef void (*logFunction)(const char*);
	void setLogFunction(logFunction f);
}
#endif
