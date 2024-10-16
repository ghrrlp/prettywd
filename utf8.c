#include "utf8.h"

static void push32(uint8_t src, int shift, uint32_t *dest) {
    *dest = (*dest << shift) | (src & ~(~0UL << shift));
}
static void pop32(uint32_t *src, int shift, uint8_t *dest) {
    *dest |= *src & ~(~0UL << shift);
    *src >>= shift;
}

codept_t dec8(const uint8_t *u) {
    int l;
    codept_t p = CODEPT_NIL;
    if (!u[0]) l = 0;
    else if ((u[0] & 0x80) == 0x00) l = 1, push32(u[0], 8, &p);
    else if ((u[0] & 0xE0) == 0xC0) l = 2, push32(u[0], 5, &p);
    else if ((u[0] & 0xF0) == 0xE0) l = 3, push32(u[0], 4, &p);
    else if ((u[0] & 0xF8) == 0xF0) l = 4, push32(u[0], 3, &p);
    else return CODEPT_INVAL;
    if (l > 1) { if (!u[1]) return CODEPT_INVAL; push32(u[1], 6, &p); }
    if (l > 2) { if (!u[2]) return CODEPT_INVAL; push32(u[2], 6, &p); }
    if (l > 3) { if (!u[3]) return CODEPT_INVAL; push32(u[3], 6, &p); }
    return p;
}
int len8(const uint8_t *u) {
    if (!*u) return 0;
    if ((*u & 0x80) == 0x00) return 1;
    if ((*u & 0xE0) == 0xC0) return 2;
    if ((*u & 0xF0) == 0xE0) return 3;
    if ((*u & 0xF8) == 0xF0) return 4;
    return -1;
}
int lenpt(codept_t p) {
    if (!p) return 0;
    if (p <= 0x00007F) return 1;
    if (p <= 0x0007FF) return 2;
    if (p <= 0x00FFFF) return 3;
    if (p <= 0x10FFFF) return 4;
    return -1;
}
int enc8(codept_t p, uint8_t *u) {
    int l;
    if (!p) l = 0;
    else if (p <= 0x00007F) l = 1, u[0] = 0x00;
    else if (p <= 0x0007FF) l = 2, u[0] = 0xC0;
    else if (p <= 0x00FFFF) l = 3, u[0] = 0xE0;
    else if (p <= 0x10FFFF) l = 4, u[0] = 0xF0;
    else l = -1;
    switch (l) {
        case -1: break;
        case 4: u[3] = 0x80, pop32(&p, 6, u + 3);
        case 3: u[2] = 0x80, pop32(&p, 6, u + 2);
        case 2: u[1] = 0x80, pop32(&p, 6, u + 1);
        case 1: pop32(&p, 8, u);
        default: u[l] = 0;
    }
    return l;
}
