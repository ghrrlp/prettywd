#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "getcwdx.h"

char *getcwdx() {
    size_t n = sizeof(char) * 1024;
    char *buf = malloc(n);
    while (!getcwd(buf, n)) {
        if (errno != ERANGE) abort();
        n <<= 1;
        if (!(buf = realloc(buf, n))) abort();
    }
    return buf;
}
