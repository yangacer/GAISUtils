#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
//#include <ext/stdio_filebuf.h>

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
	int on_searching_;
	bool found;
};

class basic_rio
{
public:
	basic_rio();
	
	basic_rio(char const *begin_pat, size_t psize);
	
	~basic_rio();

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
class irstream : public std::istream, public basic_rio
{
public:
	irstream();
	irstream(char const* pattern, size_t psize, searchablebuf *sb);
	~irstream();
	
	/**
	 * @return Constant searchable* point to buffer used by an instance.
	 */
	searchablebuf* 
	rdbuf() const;
	
	/**
	 * @param sb searchablebuf* that replaces the original one.
	 * @return The original searchblebuf*
	 */
	searchablebuf* 
	rdbuf(searchablebuf *sb);

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
	research()
	{ state_ = INITED; std::istream::clear(); rdbuf()->restart(); }

private:
	/** Prevent copy and assignment to this class
	 */
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
	
	irfstream(char const* pattern, size_t psize, FILE* c_file);

	searchablebuf_tmpl<std::filebuf > *
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
	searchablebuf_tmpl<std::filebuf> fbuf_;

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

class orstream : public std::ostream, public basic_rio
{
public:
	orstream();
	orstream(char const *begin_pat, size_t psize);
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
	orfstream(orfstream const & cp);
	irstream & operator = (orfstream const &cp);
	std::filebuf fbuf_;
};
