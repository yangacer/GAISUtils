#include "lexUtils.h"
#include <cstdlib>
#include <ctime>

struct ATOI_ ATOI;

char codebook_[60] = {
	'a', '1', 'b', 'c', '2', '0',
	'd', '*', 'J', 'p', '3', '~',
	'e', '-', 'K', 'q', '4', ')',
	'f', '/', 'L', 'r', '5', '(',
	'g', '=', 'M', 's', '9', '6',
	'h', '[', 'N', 't', '#', '7',
	'i', '^', 'O', 'u', '!', '8',
	'&', 'D', 'E', 'H', 'Q', 'T',
	'%', 'C', 'F', 'I', 'R', 'U',
	'A', 'B', 'G', 'P', 'S', 'V'
};

struct revcode{
private:
	char rcodebook_[256];
public:
	revcode()
	{
		for(int i=0;i<60;i++){
			rcodebook_[(int)codebook_[i]] = i;			
		}
	}

	char operator()(char c)
	{
		return rcodebook_[(int)c];	
	}

};

revcode RC;

std::string random_string(int length)
{
	std::string ret;
	ret.resize(length);
	
	srand((unsigned int)time(0));
	for(int i=0;i<length;i++){
		ret[i] = codebook_[rand() % 60];

	}
	return ret;
}


std::string coder::encode(std::string const &str)
{
	std::string rt;
	for(size_t i=0; i<str.size(); i++){
		rt += codebook_[(int)str[i]] ;
	}
	return rt;
}

std::string coder::decode(std::string const &str)
{
	std::string rt;
	for(size_t i=0; i<str.size(); i++){
		rt += RC((int)str[i]) ;
	}
	return rt;
}

