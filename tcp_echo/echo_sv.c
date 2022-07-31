#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include "make_daemon.h"

#define SERVICE "echo"
#define BUF_SIZE 4096
#define BACKLOG 50
#define errexit(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void
grim_reaper(int sig)        /* Reapes any dead chidren to avoid zombie processes */
{
    int saved_errno;

    saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0)
        continue;

    errno = saved_errno;
}

int
inet_listen(const char *service)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int optval, lfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;        /* IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE;        /* Wildcard IP address */

    if (getaddrinfo(NULL, SERVICE, &hints, &result) != 0)
        return -1;

    optval = 1;

    /* Walk through result list and bind to any address */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);      /* lfd is listening socket */
        if (lfd == -1)
            continue;       /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                    sizeof(optval)) == -1) {
            close(lfd);
            freeaddrinfo(result);
            return -1;
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;          /* Success */

        /* bind() failed: close socket and try next address */
        close (lfd);
    }

    if (rp != NULL) {
        if (listen(lfd, BACKLOG) == -1) {
            freeaddrinfo(result);
            return -1;
        }
    }

    freeaddrinfo(result);

    return (rp == NULL) ? -1 : lfd;
}

static void
handle_request(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t num_read;

    while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, num_read) != num_read) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (num_read == -1) {
        syslog(LOG_ERR, "Erro from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char *argv[])
{
    int lfd, cfd;
    struct sigaction sa;

    if (make_daemon(0) == -1)
        errexit("make_daemon");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_reaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {          /* Established a signal handler for SIGCHLD */
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inet_listen(SERVICE);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (;;) {
        cfd = accept(lfd, NULL, NULL);      /* Wait for connection */
        if (cfd == -1) {
            syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Each client request creates a new child process */

        switch (fork()) {
            case -1:
                syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
                close(cfd);
                break;      /* Try next client */

            case 0:         /* Child process */
                close(lfd);     /* Not needed for listening socket (since descriptors are copied during fork() */
                handle_request(cfd);
                _exit(EXIT_SUCCESS);

            default:        /* Parent process */
                close(cfd);     /* Parent process won't be connecting; all it does is listen and fork() */
                break;
        }
    }
}
