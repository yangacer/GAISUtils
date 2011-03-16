#include "mem_helper.h"

template<typename Elem>
void safeFreeBuffer(Elem** buf)
{
	//delete [] (*buf);
	free(*buf);
	*buf = 0;
}

