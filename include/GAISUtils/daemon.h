#ifndef _DAEMON_HELPER_H
#define _DAEMON_HELPER_H

#include "daemon/daemon.h"
#if defined(_WIN32)
#include "daemon/daemon_windows.h"
#elif  defined(__GNUC__)
#include "daemon/daemon_unix.h"
#endif

#endif

