#ifndef MAKE_DAEMON_H
#define MAKE_DAEMON_H 

#define D_NO_CHDIR                  01
#define D_NO_CLOSE_FILES            02
#define D_NO_REOPEN_STD_FILES       04
#define D_NO_UMASK0                 010
#define D_MAX_CLOSE                 8192

int make_daemon(int flags);

#endif
