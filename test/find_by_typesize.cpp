#include "type_manip.h"
#include "loki/Typelist.h"

int main()
{
	using namespace Loki;
	typedef TL::MakeTypelist<unsigned char, unsigned short, unsigned int, unsigned long long>::Result tlist;
	typedef type_with_eq_size<tlist, sizeof(char)>::Result Result;
}
