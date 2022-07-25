#include "server.h"
#include <netdb.h>

int
main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int cfd;
    char res_str[STR_LEN];

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usage_error("%s [hostname] [req-query]\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0)
        errexit("getaddrinfo");

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(cfd);
    }

    if (rp == NULL)
        fatal("Couldn't connect socket to any address");

    freeaddrinfo(result);

    char *req_str = argv[2];

    if (write(cfd, req_str, strlen(req_str)) != strlen(req_str))
        fatal("Partial/failed write (req_str)");
    if (write(cfd, "\n", 1) != 1)
        fatal("Partial/failed write (newline)");

    size_t numread = read_line(cfd, res_str, STR_LEN);
    if (numread == -1)
        errexit("read_line");
    if (numread == 0)
        fatal("Unexpected EOF from server");

    printf("Response: %s", res_str);

    exit(EXIT_SUCCESS);
}
