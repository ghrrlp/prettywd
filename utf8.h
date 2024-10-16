#ifndef UTF8_H
#define UTF8_H 1

#include <stdint.h>
typedef uint32_t codept_t;

#define CODEPT_NIL 0U
#define CODEPT_INVAL (~0U)

codept_t dec8(const uint8_t *);
int len8(const uint8_t *);
int lenpt(codept_t);
int enc8(codept_t, uint8_t *);

#endif /* !UTF8_H */
