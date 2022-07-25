#include "read_line.h"
#include <unistd.h>
#include <errno.h>

ssize_t
read_line(int fd, void *buffer, size_t n)
{
    ssize_t num_read;
    size_t tot_read;
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;

    tot_read = 0;
    for (;;) {
        num_read = read(fd, &ch, 1);

        if (num_read == -1) {
            if (errno == EINTR)     /* Interrupted --> restart read() */
                continue;
            else
             return -1;

        } else if (num_read == 0) {     /* EOF */
            if (tot_read == 0)          /* No bytes read; return 0 */
                return 0;
            else
             break;                     /* Some bytes read; add '\0' */

        } else {                        /* 'num_read' must be 1 if we get here */
            if (tot_read < n - 1) {     /* Discard > (n - 1) bytes */
                tot_read++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }
    *buf = '\0';
    return tot_read;
}
