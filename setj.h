#ifndef SETJ_H
#define SETJ_H 1

#include <stddef.h>
#include <stdint.h>

void setj(uint8_t *loc, size_t val);
size_t getj(const uint8_t *loc);

#endif /* !SETJ_H */
