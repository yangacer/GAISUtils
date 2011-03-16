#include "lexUtils.h"
#include <iostream>

int main(int argc, char** argv)
{
	if(argc < 3)
		exit(1);


	std::vector<std::string> args;
	for(int i = 1; i< argc-1; i++){
		args.push_back(argv[i]);
	}

	esc es(args);


	std::string src(argv[argc-1]), dest;
	
	es.escape(src);
	std::cout<<src<<"\n";
	es.unescape(src);
	std::cout<<src<<"\n";
	
	/*
	printf("escaped string: %s\n", escape);
	printf("length: %d\n", Len);
	Len = es.unescape(&unescape, escape, Len);
	printf("unescaped string: %s\n", unescape);
	printf("length: %d\n", Len);
	

	delete [] escape;
	delete [] unescape;
	*/
	return 0;
}
