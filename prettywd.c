#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "getcwdx.h"
#include "setj.h"
#include "utf8.h"

// NOTE on "initial" and "count"
// suppose we have this path
//     /home/me/Documents/projects/GitHub/small/prettywd
// the most bash/zsh does is something like the following
//     /home/me/Documents/projects/GitHub/small/prettywd
//     ~/Documents/projects/Github/small/prettywd
//     small/prettywd
// but I want something like the vim path-collapse effect
//     ~;DpGs;prettywd
//     ~1;pG;small/prettywd
//     /6;prettywd

int main(int argc, char *argv[]) {
    const size_t BAD = SIZE_MAX, INF = SIZE_MAX - 1;
    const size_t _NFULL = 1, _NINITIAL = INF;

    bool   root     = false; // prefer /home/user to ~
    bool   count    = false; // whether to show count
    size_t nfull    =   BAD; // max number of fully displayed levels
    size_t ninitial =   BAD; // max number of levels displayed as initials
    size_t mul = BAD, all_flagged = 0;

#define decr_all() ( all_flagged && --all_flagged, (void)0 )

    char *home = NULL, *wd = NULL, *wdh = NULL, *wdi = NULL, tmp;
    size_t homelen, wdlen, nparts = 0;
    bool is_home, is_following_slash = true;

    int i, j;
    char *p, *prev = NULL, *start = NULL;
    const char *c;

    if (argc > 1)
        for (c = argv[1]; *c; ++c)
            if ('0' <= *c && *c <= '9') {
                if (mul == BAD) mul = 0;
                mul = mul * 10 + (*c - '0');
            } else switch (*c) {
                case '+': all_flagged = all_flagged ? INF
                                      : mul == BAD  ? 1
                                      : mul;
                          mul = BAD; break;
                case 'r': root  =  true; decr_all(); mul = BAD; break;
                case 'R': root  = false; decr_all(); mul = BAD; break;
                case 'c': count =  true; decr_all(); mul = BAD; break;
                case 'C': count = false; decr_all(); mul = BAD; break;
                case 'f': if (nfull == BAD)    nfull = 0;
                          if (all_flagged)     nfull = INF;
                          else if (mul != BAD) nfull = mul;
                          else               ++nfull;
                          decr_all(); mul = BAD; break;
                case 'i': if (ninitial == BAD) ninitial = 0;
                          if (all_flagged)     ninitial = INF;
                          else if (mul != BAD) ninitial = mul;
                          else               ++ninitial;
                          decr_all(); mul = BAD; break;
            }
    if (nfull    == BAD) nfull    =    _NFULL;
    if (ninitial == BAD) ninitial = _NINITIAL;

    home = getenv("HOME");
    if (home) homelen = strlen(home); else root = true;
    wdlen = strlen(( wd = argc > 2 ? argv[2] : getcwdx() ));
    if (!(wdi = malloc(wdlen + 1))) abort();
    memset(wdi, 0, wdlen + 1);

    if (root || homelen > wdlen) is_home = false;
    else {
        tmp = wd[homelen]; wd[homelen] = 0; // create a 0-term'd slice
        is_home = (strcmp(wd, home) == 0);  // compare strings
        wd[homelen] = tmp;                  // restore original
    }

    wdh = is_home ? wd + homelen : wd;

    for (p = wdh, i = len8(p); i >= 1; p += i, i = len8(p))
        if (*p == '/') {
            is_following_slash = true;
        } else if (is_following_slash) {
            nparts += 1;
            is_following_slash = false;
            if (prev) setj(wdi + (prev - wd), p - prev);
            else start = p;
            prev = p;
        }
    if (i == -1) abort(); // bad UTF8

    if (nparts <= nfull || nparts - nfull <= ninitial) {
        fputc(is_home ? '~' : '/', stdout);
    } else if (count) {
        printf("%c%lu,", is_home ? '~' : '+', nparts - nfull - ninitial);
    } else {
        fputs(is_home ? "~.." : "...", stdout);
    }

    if (start) {
        for ( p = start, i = getj(wdi + (p - wd));
              nparts > nfull;
              p += i, i = getj(wdi + (p - wd)), --nparts )
            if (nparts - nfull <= ninitial)
                tmp = p[j = len8(p)], p[j] = 0, fputs(p, stdout), p[j] = tmp;
            else
                start = p + i;
        if (nparts) {
            if (p != start) fputc(';', stdout);
            fputs(p, stdout);
        }
    }

    if (argc <= 2) free(wd);
    free(wdi);
    return 0;
}
