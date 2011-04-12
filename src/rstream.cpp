#include "rstream.h"
#include <cassert>

size_t const searchablebuf::npos((size_t)-1);

//------------ basic rio class impl ------------------
basic_rio::basic_rio()
: max_rec_size_(0), pattern_(0), psize_(0)
{}


basic_rio::basic_rio(char const *begin_pat, size_t psize)
: max_rec_size_(0), pattern_(0), psize_(0)
{
	begin_pattern(begin_pat, psize);
}

basic_rio::~basic_rio()
{ delete []pattern_; }


void 
basic_rio::begin_pattern ( char const* pattern, size_t psize )
{
	if(!pattern || !psize)
		return;
	if(psize > psize_ || !psize_ ){
		delete [] pattern_;
		pattern_ = new char[psize+1];
		assert(pattern_ != 0);
		psize_ = psize;
	}
	memcpy(pattern_, pattern, psize);
	pattern_[psize] = 0;
}

size_t
basic_rio::maximal_record() const
{ return max_rec_size_; }


//------------ irstream class implementation -----------------------

irstream::irstream() 
:	std::istream(0), basic_rio(),
	state_(UNINITED) 
{}
	
irstream::irstream(char const* pattern, size_t psize, std::streambuf *sb) 
: 	basic_rio(pattern, psize),
	std::istream(dynamic_cast<std::streambuf*>(sb))
{
	init(dynamic_cast<std::streambuf*>(sb));
	state_ = INITED;
}

irstream::~irstream()
{}


void
irstream::research()
{ 
	state_ = INITED; 
	std::istream::clear(); 
	dynamic_cast<searchablebuf*>(rdbuf())->restart(); 
}

irstream & 
irstream::getrecord(char* output, size_t size)
{
	if(fail() || eof() || bad() || size == 0)
		return *this;

	// std::streambuf* stdbuf(dynamic_cast<std::streambuf*>(rdbuf()));
	searchablebuf* mybuf(dynamic_cast<searchablebuf*>(rdbuf()));
	assert(mybuf != 0);
	// touch
	rdbuf()->sgetc();

	// preserve null terminator character
	size -= 1;

	size_t pos(0);
	size_t min(0);
	
	this->output_off = 0;

	Dispatch:
	switch(state_){
		case INITED:
			// Match preceding
			while( !fail() && !eof() && !bad() ){

				pos = mybuf->search(begin_pattern(), psize());

				if( searchablebuf::npos == pos){
					// Flush whole buffer and refill it
					ignore(rdbuf()->in_avail());
					peek();
				}else{
					ignore(pos + psize());
					state_ = PMATCH;
					goto Dispatch;
				}
			}
			break;
		case PMATCH:
			pos = mybuf->search(begin_pattern(), psize());
			// Not found
			if( searchablebuf::npos == pos ){
				// output buffer available
				if(output_off < size){
					min = std::min(size - output_off, (size_t)rdbuf()->in_avail());
					read(output + output_off, min);
					output_off += min;
					peek();
					if(eof()){
						output[output_off] = 0;
						basic_rio::max_rec_size_ = std::max(output_off, maximal_record());
						return *this;
					}
					goto Dispatch;
				}
				// output buffer overflow (fail)
				else {
					output_off = 0;
					ignore(rdbuf()->in_avail());
					peek();
					state_ = PMATCH;
					setstate(ios_base::failbit);
					return *this;
				}
			}
			// Found but output buffer overflow
			else if(pos > size - output_off){
				// allow client to resume fetch followed data with larger output buffer
				read(output + output_off, size-output_off);
				output_off = size;
				peek();
				state_ = PMATCH;
				setstate(ios_base::failbit);
				return *this;
			}
			// Found and output buffer available
			else{
				read(output + output_off, pos);
				output[output_off + pos] = 0;
				output_off += pos;
				ignore(psize());
				state_ = PMATCH;
				basic_rio::max_rec_size_ = std::max(output_off, maximal_record());
				return *this;
			}
			break;

	}
	return *this;
}

size_t
irstream::getrecord(char const** beg)
{
	if(fail() || eof() || bad())
		return 0;

	// std::streambuf* stdbuf(dynamic_cast<std::streambuf*>(rdbuf()));
	searchablebuf* mybuf(dynamic_cast<searchablebuf*>(rdbuf()));
	assert(mybuf != 0);

	// touch
	rdbuf()->sgetc();

	char const* end(0);
	
	Dispatch:
	switch(state_){
		case INITED:
			// Match preceeding
			if( 0 == mybuf->search_nptr(begin_pattern(), psize()) ){
				setstate(ios_base::failbit);
				*beg = 0;
				return 0;
			}else{
				state_ = PMATCH;
				goto Dispatch;
			}
			break;
		case PMATCH:
			*beg = mybuf->ptr_head();
			end = mybuf->search_nptr(begin_pattern(), psize());
			if( end == 0 ){
				setstate(ios_base::failbit);
				return mybuf->buf_end() - *beg;			
			}
			output_off = end - *beg;
			basic_rio::max_rec_size_ = std::max(output_off, maximal_record());
			return end - *beg;
			break;

	}
	return 0;
}

//----------- irfstream implementation -------------------
irfstream::irfstream() 
: irstream(), fbuf_(new searchablebuf_tmpl<std::filebuf>()) 
{
	init(fbuf_);
	irstream::rdbuf(fbuf_);
}

irfstream::~irfstream() {}

irfstream::irfstream(char const* pattern, size_t psize, 
	char const *filename, std::ios_base::openmode mode)
: irstream(), 
  fbuf_(new searchablebuf_tmpl<std::filebuf>())
{
	begin_pattern(pattern, psize);
	init(fbuf_);
	open(filename, mode);
	irstream::rdbuf(fbuf_);
}

irfstream::irfstream(char const* pattern, size_t psize, 
	FILE* c_file, std::ios_base::openmode mode, size_t bsize)
: irstream(), 
  fbuf_(new searchablebuf_tmpl<__gnu_cxx::stdio_filebuf<char> >(c_file, mode, bsize))
{
	begin_pattern(pattern, psize);
	init(fbuf_);
	irstream::rdbuf(fbuf_);
}


std::filebuf*
irfstream::rdbuf() const
{ return const_cast<std::filebuf*>(dynamic_cast<std::filebuf*>(fbuf_)); }


bool 
irfstream::is_open()
{ return rdbuf()->is_open(); }

bool 
irfstream::is_open() const
{ return rdbuf()->is_open(); }

void 
irfstream::open(char const* filename, std::ios_base::openmode mode)
{
	if(!rdbuf()->open(filename, mode | ios_base::in))
		setstate(ios_base::failbit);
	else
		clear();
	
}

void 
irfstream::close()
{
	research();
	if(!rdbuf()->close())
		setstate(ios_base::failbit);
}

//---------------- irstringstream implementation ----------------

irstringstream::irstringstream() 
: irstream(), strbuf_()
{}

irstringstream::irstringstream(char const* pattern, size_t psize)
: irstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	irstream::rdbuf(rdbuf());
}

irstringstream::irstringstream(char const* pattern, size_t psize, 
	std::string const & s)
: irstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	irstream::rdbuf(rdbuf());
	strbuf_.str(s);
}

irstringstream::~irstringstream()
{}

searchablebuf_tmpl<std::stringbuf> *
irstringstream::rdbuf() const
{ return const_cast<searchablebuf_tmpl<std::stringbuf>*>(&strbuf_); }

std::string
irstringstream::str() const
{ return strbuf_.str(); }

void
irstringstream::str(std::string & s)
{ strbuf_.str(s); }


// ---------------- orstream impl -------------------

orstream::orstream()
: std::ostream(), basic_rio()
{}

orstream::orstream(char const *begin_pat, size_t psize, std::streambuf *sb)
: std::ostream(dynamic_cast<std::streambuf*>(sb)), 
  basic_rio(begin_pat, psize)
{}

orstream::~orstream()
{}

// --------------- orfstream impl --------------------

orfstream::orfstream() 
: orstream(), fbuf_(new searchablebuf_tmpl<std::filebuf>()) 
{
	init(fbuf_);
	orstream::rdbuf(fbuf_);
}

orfstream::~orfstream() {}

orfstream::orfstream(char const* pattern, size_t psize, 
	char const *filename, std::ios_base::openmode mode)
: orstream(), 
  fbuf_(new searchablebuf_tmpl<std::filebuf>())
{
	begin_pattern(pattern, psize);
	init(fbuf_);
	open(filename, mode);
	orstream::rdbuf(fbuf_);
}

orfstream::orfstream(char const* pattern, size_t psize, 
	FILE* c_file, std::ios_base::openmode mode, size_t bsize)
: orstream(), 
  fbuf_(new searchablebuf_tmpl<__gnu_cxx::stdio_filebuf<char> >(c_file, mode, bsize))
{
	begin_pattern(pattern, psize);
	init(fbuf_);
	orstream::rdbuf(fbuf_);
}


std::filebuf*
orfstream::rdbuf() const
{ return const_cast<std::filebuf*>(dynamic_cast<std::filebuf*>(fbuf_)); }


bool 
orfstream::is_open()
{ return rdbuf()->is_open(); }

bool 
orfstream::is_open() const
{ return rdbuf()->is_open(); }

void 
orfstream::open(char const* filename, std::ios_base::openmode mode)
{
	if(!rdbuf()->open(filename, mode | ios_base::out))
		setstate(ios_base::failbit);
	else
		clear();
	
}

void 
orfstream::close()
{
	if(!rdbuf()->close())
		setstate(ios_base::failbit);
}

//---------------- orstringstream implementation ----------------

orstringstream::orstringstream() 
: orstream(), strbuf_()
{}

orstringstream::orstringstream(char const* pattern, size_t psize)
: orstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	orstream::rdbuf(rdbuf());
}

orstringstream::orstringstream(char const* pattern, size_t psize, 
	std::string const & s)
: orstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	orstream::rdbuf(rdbuf());
	strbuf_.str(s);
}

orstringstream::~orstringstream()
{}

searchablebuf_tmpl<std::stringbuf>*
orstringstream::rdbuf() const
{ return const_cast<searchablebuf_tmpl<std::stringbuf>*>(&strbuf_); }

std::string
orstringstream::str() const
{ return strbuf_.str(); }

void
orstringstream::str(std::string & s)
{ strbuf_.str(s); }


//------- rstream impl --------------

rstream::rstream()
:irstream(), orstream(0, 0, irstream::rdbuf())
{}

rstream::rstream(char const *begin_pat, size_t psize, std::streambuf* sb)
: irstream(begin_pat, psize, sb), 
  orstream(begin_pat, psize, sb)
{}

rstream::~rstream()
{}

//---------- rfstream impl ---------------

rfstream::rfstream() 
: rstream(), fbuf_() 
{
	init(dynamic_cast<std::streambuf*>(&fbuf_));
	rstream::rdbuf(&fbuf_);
}

rfstream::~rfstream() {}

rfstream::rfstream(char const* pattern, size_t psize, 
	char const *filename, std::ios_base::openmode mode)
: rstream(), fbuf_()
{
	begin_pattern(pattern, psize);
	init(dynamic_cast<std::streambuf*>(&fbuf_));
	open(filename, mode);
	rstream::rdbuf(&fbuf_);
}



searchablebuf_tmpl<std::filebuf> *
rfstream::rdbuf() const
{ return const_cast<searchablebuf_tmpl<std::filebuf> *>(&fbuf_); }


bool 
rfstream::is_open()
{ return fbuf_.is_open(); }

bool 
rfstream::is_open() const
{ return fbuf_.is_open(); }

void 
rfstream::open(char const* filename, std::ios_base::openmode mode)
{
	if(!fbuf_.open(filename, mode | ios_base::in | ios_base::out))
		setstate(ios_base::failbit);
	else
		clear();
}

void 
rfstream::close()
{
	irstream::research();
	if(!fbuf_.close())
		setstate(ios_base::failbit);
}

//---------------- rstringstream implementation ----------------

rstringstream::rstringstream() 
: rstream(), strbuf_()
{}

rstringstream::rstringstream(char const* pattern, size_t psize)
: rstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	rstream::rdbuf(rdbuf());
}

rstringstream::rstringstream(char const* pattern, size_t psize, 
	std::string const & s)
: rstream(), strbuf_()
{
	begin_pattern(pattern, psize);
	init(&strbuf_);
	rstream::rdbuf(rdbuf());
	strbuf_.str(s);
}

rstringstream::~rstringstream()
{}

searchablebuf_tmpl<std::stringbuf>*
rstringstream::rdbuf() const
{ return const_cast<searchablebuf_tmpl<std::stringbuf>*>(&strbuf_); }

std::string
rstringstream::str() const
{ return strbuf_.str(); }

void
rstringstream::str(std::string & s)
{ strbuf_.str(s); }

#ifdef TEST_RSTREAM
//----------- test main() ------------------------

#include <iostream>

int main(int argc, char** argv)
{
	using namespace std;
	
	irstringstream rsin("@\n", 2, "@\n@t:string\n@a:\n");
	

	char rec[31];
	
	
	rsin.getrecord(rec, 30);
	cerr<<"fbge\n";
	cerr<<rsin.fail()<<rsin.bad()<<rsin.good()<<rsin.eof()<<endl;
	cout<<rec<<endl;
	

	irfstream rfin;
	// Note: streambuf::pubsetbuf after opening a file will
	// make other methods have undefined behavior. Thus, I
	// use null constructor here.
	rfin.rdbuf()->pubsetbuf(new char[40], 40);
	rfin.begin_pattern("@\n", 2);
	rfin.open(argv[1]);

	assert(rfin.is_open() == true);
	
	cout<<"-------- auto recover read --------"<<endl;
	while(!rfin.eof()){
		rfin.getrecord(rec, 30);
		cerr<<"fbge\n";
		cerr<<rfin.fail()<<rfin.bad()<<rfin.good()<<rfin.eof()<<endl;
		if(!rfin.fail())
			cout<<rec<<endl;
		else{
			rfin.clear();
			cout<<"fail bit cleared"<<endl;
		}
	}
	rfin.close();

	// boundary case
	
	irfstream rfin2("", 0, argv[1]);
	rfin2.getrecord(rec, 30);
	cout<<"--------- zero pattern ---------\n";
	// Note: The search algorithm always matches a zero-length pattern.
	// Thus, client may form a eternal loop within a zero-length pattern.
	cerr<<"fbge\n";
	cerr<<rfin2.fail()<<rfin2.bad()<<rfin2.good()<<rfin2.eof()<<endl;
	cout<<rec<<endl;
	rfin2.close();
	
	
	irfstream rfin2_1;
	rfin2_1.rdbuf()->pubsetbuf(new char[20], 20);
	rfin2_1.open(argv[1]);
	rfin2_1.begin_pattern("@\n@t:record larger t\n@\n@t:", 26);
	rfin2_1.getrecord(rec, 100);
	cout<<"-------- overflow pattern ---------\n";
	// Note: The overflow means pattern size is longer than buffer size
	// and results in matching never happen.
	cerr<<"fbge\n";
	cerr<<rfin2_1.fail()<<rfin2_1.bad()<<rfin2_1.good()<<rfin2_1.eof()<<endl;
	cout<<rec<<endl;
	rfin2_1.close();


	irfstream rfin3("@\n", 2, argv[1]);
	rfin3.getrecord(rec, 0);
	cout<<"-------- zero output ---------\n";
	cerr<<"fbge\n";
	cerr<<rfin3.fail()<<rfin3.bad()<<rfin3.good()<<rfin3.eof()<<endl;
	cout<<rec<<endl;
	rfin3.close();

	irfstream rfin4("@\n", 2, argv[1]);
	char lrec[100];
	cout<<"-------- sufficient large output ---------\n";
	while(!rfin4.eof()){
		rfin4.getrecord(lrec, 100);
		cerr<<"fbge\n";
		cerr<<rfin4.fail()<<rfin4.bad()<<rfin4.good()<<rfin4.eof()<<endl;
		cout<<lrec<<endl;
	}
	rfin4.close();

	return 0;
}

#endif

