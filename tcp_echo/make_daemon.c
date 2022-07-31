#include <unistd.h>
#include "make_daemon.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/* Returns 0 on success; -1 on error */
int
make_daemon(int flags)
{
    int fd, max_fd;

    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)     /* Process becomes a session leader */
        return -1;

    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    /* The process is not a session leader any more 
     * This allows us to be sure that it wont 
     * have a controlling terminal*/

    if (!(flags & D_NO_UMASK0))     /* Set permission mask */
        umask(0);

    if (!(flags & D_NO_CHDIR))      /* Change directory to root */
        chdir("/");             

    if (!(flags & D_NO_CLOSE_FILES)) {    /* Close all existing open files */
        max_fd = sysconf(_SC_OPEN_MAX);
        if (max_fd == -1)
            max_fd = D_MAX_CLOSE;

        for (int i = 0; i < max_fd; i++)
            close(i);
    }

    if (!(flags & D_NO_REOPEN_STD_FILES)) {     /* Redirect STD* to "/dev/null" */
        fd = close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);

        if (fd != 0)                /* fd must be 0 */
            return -1;                           
        if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(fd, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}
