#include "daemon.h"
#if defined(__WIN32__)
#include "daemon_windows.h"
#elif  defined(__GNUC__)
#include "daemon_unix.h"
#endif

#include <cstdio>
#include <unistd.h>
#include <fstream>
int main(int argc, char** argv)
{
	OmniEvents::Daemon dm(argc, argv);
	dm.pidfile("/tmp/ace_dae");
	dm.tracefile("/tmp/ace_dae_trc");
	dm.daemonize();
	dm.runningOk();
	
	std::ofstream fout("/tmp/wrtout");
	for(int i=0; i<argc; i++){
		fout<<argv[i]<<std::endl;
		//printf("%s\n", argv[i]);
	}

	//dm.runningOk();
	
	return 0;
}

