#include "setj.h"

static const int K = (SIZE_MAX == UINT64_MAX) ? 64 - 8 : 32 - 8;
static const size_t A = 0x40, B = 0x80, C = 0xC0, F = 0xFF;
static const size_t X = (size_t)0x40 << K;

void setj(uint8_t *k, size_t v) {
    while (v > X) k[0] = C, k += X, v -= X;
    if (v < A) k[0] = v;
    else if (v < X) {
        k[0] = B | v >> K;
        for (int i = 1, j = K - 8; j >= 0; ++i, j -= 8) k[i] = v >> j & F;
    } else k[0] = A;
}

size_t getj(const uint8_t *k) {
    size_t v = 0;
    while (k[0] == C) k += X, v += X;
    if (k[0] == A) return v + X;
    if (k[0] < A) return v + k[0];
    v |= (size_t)k[0] << K & (~X + 1);
    for (int i = 1, j = K - 8; j >= 0; ++i, j -= 8) v |= (size_t)k[i] << j;
    return v;
}
