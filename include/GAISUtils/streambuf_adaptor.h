#ifndef _STREAMBUF_ADAPTOR_H
#define _STREAMBUF_ADAPTOR_H

#include <fstream>
#include <sstream>

#ifdef __GNUC__
#include <ext/stdio_filebuf.h>
#define HAS_GNU_EXT_STDIO_FBUF
#define GNU_STDIO_FBUF_CLASS __gnu_cxx::stdio_filebuf<char>
#endif

class streambuf_adaptor
{
public:
	virtual size_t
	max_size() const = 0;
};

template<class Streambuf>
class streambuf_adaptor_tmpl;

template<>
class streambuf_adaptor_tmpl<std::filebuf> : 
#ifdef HAS_GNU_EXT_STDIO_FBUF
	public GNU_STDIO_FBUF_CLASS
#else
	public std::filebuf
#endif
{
	
public:
	streambuf_adaptor_tmpl(){}

#ifdef HAS_GNU_EXT_STDIO_FBUF
	streambuf_adaptor_tmpl(FILE * c_file,  std::ios::openmode mode, size_t size)
	: GNU_STDIO_FBUF_CLASS(c_file, mode, size)
	{}
#endif
	size_t
	max_size() const
	{ 
#ifdef __GNUC__
		return _M_buf_allocated ? _M_buf_size : _M_ext_buf_size ;
#else
		return 0;
#endif
	}
};

template<>
class streambuf_adaptor_tmpl<std::stringbuf> : public std::stringbuf
{
	size_t
	max_size() const
	{
#ifdef __GNUC__
		return _M_string.max_size();
#else
		return 0;
#endif
	}
};

namespace GUtils {
	typedef streambuf_adaptor_tmpl<std::filebuf> filebuf;
	typedef streambuf_adaptor_tmpl<std::stringbuf> stringbuf;
}

#endif
