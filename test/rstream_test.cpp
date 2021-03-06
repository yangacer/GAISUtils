#include "GAISUtils/rstream.h"
#include "GAISUtils/streambuf_adaptor.h"
#include <iostream>

void usage()
{
	printf("rstream_test file\n");	
}

int main(int argc, char** argv)
{
	char buffer[10];
	irfstream rfin("@\n", 2, argv[1]);
	rfin.rdbuf()->pubsetbuf(buffer, 10);

	if(!rfin.is_open()){
		usage();
		return 0;
	}
	
	char record[10];
	char larger[100];
	while(!rfin.eof()){
		rfin.getrecord(record, 10);
		// given size too small
		if(rfin.fail()){
			int last_grcnt = rfin.grcount();
			rfin.clear();
			// copy read data from record
			strncpy(larger, record, rfin.grcount());
			// resume read
			rfin.getrecord(larger+rfin.grcount(), 100 - rfin.grcount());
			printf(larger);
			printf("grcount: %d\n", last_grcnt + rfin.grcount());
		}else{
			printf(record);
			printf("grcount: %d\n", rfin.grcount());
		}
		printf("---\n");
	}
	/*
	rfin.close();
	rfin.open(argv[1]);
	*/
	rfin.seekg(0, std::ios::beg);
	rfin.sync();
	rfin.research();

	char const* rptr;
	size_t size;
	size_t read=0;
	// With getrecord(rptr), the eof() will never be triggered by the method
	// since it does not change stream buffer state related to I/O.
	// Thus we use successive fail() to detect the last record instead of eof().
	while(1){
		size = rfin.getrecord(&rptr);
		if(rfin.fail()){
			// buffer exhusted
			rfin.ignore(read);
			read = 0;
			rfin.clear();
			size = rfin.getrecord(&rptr);
			// the last record reached
			if(rfin.fail()){
				printf("%d\n", size);
				break;
			}
		}
		printf("%d\n", rfin.grcount());
		read+= rfin.grcount();
	}

	rfin.close();
	
	/*
	irfstream fin2("@\n", 2, stdin);
	fin2.getrecord(record, 10);
	printf(record);
	
	fin2.close();
	fin2.open(argv[1]);
	fin2.getrecord(record, 10);
	printf("\nOpen file after read from stdin: %d\n", fin2.is_open());
	printf(record);
	*/

	irstringstream sin("@\n", 2, "@\n@s:5467\n");
	
	FILE* fp = fopen(argv[1], "rb");
	setvbuf(fp, new char[1024], _IOFBF, 1024);
	GUtils::filebuf fb(fp, std::ios::in, 1024);
	printf("buf size: %d\n", fb.max_size());

	return 0;	
}
