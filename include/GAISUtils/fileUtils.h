#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#include <vector>
#include <fstream>
#include <string>
#include "container.h"

#include "dll.h"

/// Partition a file into multiple files of given size
struct DLL_PUBLIC spliter
{
	/// @param bufsize Set internal used buffer size, 0 for default size (8MB)
	/** @remark Buffer size is large as 8M at least
	 */
	spliter(unsigned int bufsize = 0);
	
	/**
	 * @param result For placing resulted filename
	 * @param size Size for each partition
	 * @param filename File to be partitioned
	 * @return 0 if failed, size of last partition if successed
	 */
	int operator()(std::vector<std::string>& result, unsigned int size, char const* filename);
private:
	circular_buffer<char> buf_;
};

#endif
