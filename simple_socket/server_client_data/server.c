#define _DEFAULT_SOURCE

#include <netdb.h>
#include "server.h"
#include "../database/database.h"
#include <arpa/inet.h>

#define ADDSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
#define BACKLOG 50

void 
make_null_terminated(char *old_string, char *new_string)
{
    while (*old_string != '\n') {
        *new_string++ = *old_string++;
    }
    *new_string = '\0';
}

void parse_req(char *req_query, int *opt, char *name, int *value)
{
    int flag = 0;       /* this flag is used for assigning different values according to the place in query */
    char *str1, *str2, *token, *subtoken;
    char *str1pnt, *str2pnt;
    char new_req_str[STR_LEN];

    make_null_terminated(req_query, new_req_str);


    for (str1 = new_req_str; ; str1 = NULL) {
        token = strtok_r(str1, "&", &str1pnt);
        if (token == NULL)
            break;
        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, "=", &str2pnt);
            if (subtoken == NULL)
                break;
            switch (flag) {
                case 0: flag++; break;
                case 1: 
                        if (strcmp(subtoken, "add") == 0)
                            *opt = 1;
                        else if (strcmp(subtoken, "del") == 0)
                            *opt = 2;
                        else if (strcmp(subtoken, "mod") == 0)
                            *opt = 3;
                        else 
                             *opt = 4;
                       flag++; 
                       break;
                case 2: strcpy(name, subtoken); flag++; break;
                case 3: *value = atoi(subtoken); flag++; break;
                default: break;
            }
        }
    }
}

size_t 
db_storage(struct DB **head, char *req_query, char *res)
{
    int opt, value, ret_value;
    char name[10];
    char ret_name[10];
    parse_req(req_query, &opt, name, &value);

    switch (opt) {
        case 1:      /* Add */
            add_db(head, name, 10, value);
            snprintf(res, STR_LEN, "Added %s to db\n", (*head)->name);
            break;
        case 2:     /* Delete */
            if (del_db(*head, name) == -1)
                snprintf(res, STR_LEN, "ERROR: Couldn't find record\n");
            else
             snprintf(res, STR_LEN, "Deleted record\n");
            break;
        case 3:     /* Modify */
            if (mod_db(*head, name, value) == -1)
                snprintf(res, STR_LEN, "Error: No record to modify\n");
            else
             snprintf(res, STR_LEN, "Data modified\n");
            break;
        case 4:     /* Retrieve */
            if (head == NULL) {
                break;
            }
            struct DB *curr;
            if (curr == NULL)
                printf("NULL\n");
            if (ret_db(*head, name, ret_name, &ret_value) == -1)
                snprintf(res, STR_LEN, "Couldn't retrieve: No Record\n");
            else
             snprintf(res, STR_LEN, "%s = %d\n", ret_name, ret_value);
            break;
        default:
            snprintf(res, STR_LEN, "Option error\n");
            break;
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char req_str[STR_LEN];
    char addr_str[ADDSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char res_str[STR_LEN];
    int sfd, cfd, optval, req_int, req_cnt;
    struct sockaddr_storage cl_addr;
    socklen_t cl_addrlen;
    struct DB *head = NULL;         /* Points to the first element in DB list */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)        /* Ignore SIGPIPE */
        errexit("signal");

    memset(&hints, 0, sizeof(hints));
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;        /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;       /* Wildcard address */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
        errexit("getaddrinfo");

    optval = 1;
    
    /* Walk through the result list until we successfully bind to a result */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;       /* If failed; try next address */

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            errexit("setsocket");

        //char buf[20];
        //inet_ntop(rp->ai_family, &((struct sockaddr_in *)rp->ai_addr)->sin_addr, buf, rp->ai_addrlen);
        //printf("address: %s\n", buf);

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;          /* Success */

        close(sfd);         /* If couldn't bind, close the fd and try next address */
    }

    if (rp == NULL) {           /* Couldn't bind to any address */
        freeaddrinfo(result);
        errexit("Couldn't bind to any address");
    }

    if (listen(sfd, BACKLOG) == -1)
        errexit("listen");

    freeaddrinfo(result);       /* Since we have no use of result anymore */
    printf("Server has started listening at %s\n", PORT_NUM);
    req_cnt = 0;

    for (;;) {        /* Clients are handled iteratively */
        cl_addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(sfd, (struct sockaddr *) &cl_addr, &cl_addrlen);
        if (cfd == -1) {
            err_msg("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &cl_addr, cl_addrlen, host, NI_MAXHOST,
                    service, NI_MAXSERV, 0) == -1)
            snprintf(addr_str, ADDSTRLEN, "(?UNKNOWN?)");
        else
         snprintf(addr_str, ADDSTRLEN, "(%s, %s)", host, service);

        printf("Connection from %s\n", addr_str);

        if (read_line(cfd, req_str, STR_LEN) <= 0) {
            perror("read_line");
            close(cfd);
            continue;           /* Read failed, skip */
        }

        db_storage(&head, req_str, res_str);

        if (write(cfd, res_str, strlen(res_str)) != strlen(res_str))
            fatal("failed/partial write");

        req_cnt++;
        printf("Request compleletd=%d\n", req_cnt);

        if (close(cfd) == -1)
            errexit("cfd");
    }
}
