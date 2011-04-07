#ifndef _RSTREAM_H
#define _RSTREAM_H

#include <cassert>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include "ext/stdio_filebuf.h"

/** Searchable buffer abstract class
 *  This class can not be instantaneoused
 */
class searchablebuf 
{
public:
	/** No position value.
	*/
	static size_t const npos;

	/** Pattern search interface.
	 *  No default implementation due to methods required are not 
	 *  available in this class.
	 *  @see class searchablebuf_tmpl
	 */
	virtual size_t 
	search(char const* pattern, int psize) = 0;

	virtual char const*
	search_nptr(char const* pattern, int psize) = 0;

	virtual char const*
	ptr_head() = 0;
	
	virtual char const*
	buf_end() = 0;

	virtual void
	restart() = 0;
};

/** Template for concrete classes derived from std::streambuf.
 */
template<class StreamBuf>
class searchablebuf_tmpl : public StreamBuf, public searchablebuf
{
public:
	searchablebuf_tmpl()
	: on_searching_(0), found(true)
	{}
	
	searchablebuf_tmpl(FILE* c_file, std::ios::openmode mode, size_t size)
	: StreamBuf(c_file, mode, size), // failure when derived classes have no such ctor
	  on_searching_(0), found(true)
	{}

	/** Generic pattern search implementation.
	 * @param pattern Record begin pattern.
	 * @param psize Size of record begin pattern.
	 * @return Position of the first occurrence or npos if pattern hasn't found.
	 */
	size_t 
	search(char const* pattern, int psize)
	{
		if(StreamBuf::in_avail() < psize)
			return npos;
		char* ptr(std::search(StreamBuf::gptr(), StreamBuf::egptr(), pattern, pattern + psize));
		return (ptr != StreamBuf::egptr() ) ? ptr - StreamBuf::gptr() : npos ;
			
	}

	char const*
	search_nptr(char const *pattern, int psize)
	{
		if(!found)
			return 0;

		char* ptr( std::search(StreamBuf::gptr() + on_searching_, 
			StreamBuf::egptr(), pattern, pattern + psize) );
		char *tmp(StreamBuf::egptr());
		if(ptr == StreamBuf::egptr()){
			found = false;
			return 0;
		}
		
		on_searching_ = ptr - StreamBuf::gptr() + psize;
		return ptr;
	}

	char const*
	ptr_head()
	{ return StreamBuf::gptr() + on_searching_; }

	char const*
	buf_end()
	{ return StreamBuf::egptr(); }

	void
	restart()
	{ found = true; on_searching_ = 0; }

private:
	/** Prevent copy and assignment to this class
	 */
	searchablebuf_tmpl(searchablebuf_tmpl const &cp);
	searchablebuf_tmpl& operator = (searchablebuf_tmpl const & cp);
	unsigned int on_searching_;
	bool found;
};


class basic_rio
{
public:
	basic_rio();
	
	basic_rio(char const *begin_pat, size_t psize);
	
	virtual ~basic_rio();

	/** 
	 * @return Pattern size.
	 */
	size_t 
	psize() const
	{ return psize_; }
	
	/** 
	 * @return C-string pattern.
	 */
	char const * 
	begin_pattern () const
	{ return pattern_; }
	
	/** 
	 * @param pattern const C-string pattern.
	 * @param size Size of pattern parameter.
	 */
	void 
	begin_pattern ( char const* pattern, size_t psize );
	
		
private:
	basic_rio(basic_rio const &cp);
	basic_rio& operator=(basic_rio const &cp);

	char *pattern_;
	size_t psize_;
};


/** Input record stream
 */
class irstream : public std::istream, virtual public basic_rio
{
public:
	irstream();
	irstream(char const* pattern, size_t psize, std::streambuf *sb);
	virtual ~irstream();
	
	/**
	 * @param output For placing fetched record.
	 * @param size Size of output parameter.
	 * @return *this.
	 * @remark Failure of this method set the failbit that returned via fail() method.
	 * Clients can check whether a getrecord fail as follows:
	 * @codebeg
	 * irfstream fin("beg_pattern", strlen("beg_pattern"), filename);
	 * char* buf[1024];
	 * if(fin.getrecrod(buf, 1024)) // Evaluated to false when failure happened.
	 * ...
	 * @endcode
	 * or
	 * @codebeg
	 * fin.getrecord(buf, 1024);
	 * if(fin.fail())
	 * ...
	 * @endcode
	 */
	irstream &
	getrecord(char* output, size_t size);
	
	/** 
	 * @param beg For placing found pointer points to record begin
	 * @return Size of found record
	 * @remark This method does NOT change any buffer states. When
	 * searching fail, buffer margin reached, return value will be
	 * the distance between buffer end pointer (eqptr()) and previous
	 * record's end or buffer begin pointer (gptr()).
	 */
	size_t
	getrecord(char const** beg);

	/** 
	 * @return Size of the last successful getrecord
	 * @remark This is NOT equal to std::istream::gcount()
	 */
	size_t 
	grcount() const
	{ return output_off; }
	
	void
	research();

private:
	irstream(irstream const & cp);
	irstream & operator = (irstream const &cp);

	enum STATE{ INITED, PMATCH, UNINITED };
	STATE state_;
	size_t output_off;
};

class irfstream : public irstream
{
public:
	irfstream();
	~irfstream();

	irfstream(char const* pattern, size_t psize, 
		char const *filename, std::ios_base::openmode mode = ios_base::in);
	
	irfstream(char const* pattern, size_t psize, 
		FILE* c_file, std::ios_base::openmode mode, size_t bsize);

	std::filebuf*
	rdbuf() const;

	bool 
	is_open();
	
	bool 
	is_open() const;

	void 
	open(char const* filename, std::ios_base::openmode mode = ios_base::in);

	void 
	close();

private:
	irfstream(irfstream const & cp);
	irfstream & operator = (irfstream const &cp);
	std::streambuf* fbuf_;

};

class irstringstream : public irstream 
{
public:
	irstringstream();

	/** Notice the strbuf_ is initialized with ios::in | ios::out
	 */
	irstringstream(char const* pattern, size_t psize);
	irstringstream(char const* pattern, size_t psize, 
		std::string const & s);
	~irstringstream();

	searchablebuf_tmpl<std::stringbuf> *
	rdbuf() const;

	std::string
	str() const;
	
	void
	str(std::string & s);

private:
	irstringstream(irstringstream const &cp);
	irstringstream& operator=(irstringstream const &cp);

	searchablebuf_tmpl<std::stringbuf> strbuf_;
};

// -------------- orstream decl -------------------

class orstream : public std::ostream, virtual public basic_rio
{
public:
	orstream();
	orstream(char const *begin_pat, size_t psize, std::streambuf* sb);
	virtual ~orstream();
	
private:
	orstream(orstream const &cp);
	orstream& operator=(orstream const &cp);
};

class orfstream : public orstream
{
public:
	orfstream();
	~orfstream();

	orfstream(char const* pattern, size_t psize, 
		char const *filename, std::ios_base::openmode mode = ios_base::in);
	
	orfstream(char const* pattern, size_t psize, FILE* c_file);

	searchablebuf_tmpl<std::filebuf>*
	rdbuf() const;

	bool 
	is_open();
	
	bool 
	is_open() const;

	void 
	open(char const* filename, std::ios_base::openmode mode = ios_base::in);

	void 
	close();

private:
	orfstream(orfstream const & cp);
	irstream & operator = (orfstream const &cp);
	searchablebuf_tmpl<std::filebuf> fbuf_;
};

class orstringstream : public orstream 
{
public:
	orstringstream();

	/** Notice the strbuf_ is initialized with ios::in | ios::out
	 */
	orstringstream(char const* pattern, size_t psize);
	orstringstream(char const* pattern, size_t psize, 
		std::string const & s);
	~orstringstream();

	searchablebuf_tmpl<std::stringbuf>*
	rdbuf() const;

	std::string
	str() const;
	
	void
	str(std::string & s);

private:
	orstringstream(orstringstream const &cp);
	orstringstream& operator=(orstringstream const &cp);

	searchablebuf_tmpl<std::stringbuf> strbuf_;
};

// -------------- rstream --------------------

class rstream : public irstream, public orstream
{
public:
	rstream();
	rstream(char const *begin_pat, size_t psize, std::streambuf* sb);
	virtual ~rstream();
private:
	rstream(rstream const& cp);
	rstream& operator=(rstream const& cp);
};


class rfstream : public rstream
{
public:
	rfstream();
	rfstream(char const *begin_pat, size_t psize);
	~rfstream();
	
	rfstream(char const* pattern, size_t psize, 
		char const *filename, 
		std::ios_base::openmode mode = ios_base::in | ios_base::out);
	
	rfstream(char const* pattern, size_t psize, FILE* c_file);

	searchablebuf_tmpl<std::filebuf>*
	rdbuf() const;

	bool 
	is_open();
	
	bool 
	is_open() const;

	void 
	open(char const* filename, 
		std::ios_base::openmode mode = ios_base::in | ios_base::out);

	void 
	close();

private:
	rfstream(rfstream const& cp);
	rfstream& operator=(rfstream const& cp);
	searchablebuf_tmpl<std::filebuf> fbuf_;
};

class rstringstream : public rstream 
{
public:
	rstringstream();

	/** Notice the strbuf_ is initialized with ios::in | ios::out
	 */
	rstringstream(char const* pattern, size_t psize);
	rstringstream(char const* pattern, size_t psize, 
		std::string const & s);
	~rstringstream();

	searchablebuf_tmpl<std::stringbuf>*
	rdbuf() const;

	std::string
	str() const;
	
	void
	str(std::string & s);

private:
	rstringstream(rstringstream const &cp);
	rstringstream& operator=(rstringstream const &cp);

	searchablebuf_tmpl<std::stringbuf> strbuf_;
};
#endif
