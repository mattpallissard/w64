#ifndef H_HDFUN
#define H_HDFUN
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum { W_LEN = sizeof(uint64_t),
       MAX_INT = ~0x0,
       flush = 0,
};
uint64_t unpack(uint8_t *d, size_t l);
uint8_t *prep();
bool check_byte(uint64_t d, uint8_t i);
bool check_word(uint8_t *d, size_t s);
void fill(uint8_t *d, size_t s);
void input(uint8_t *d, const char *p, size_t s);
void output(uint8_t *d, const char *p, size_t s, bool f);
void pack(uint8_t *d, uint64_t s, size_t l);
void printb(uint64_t i);
void run(uint8_t *s, uint8_t *d, const char *p, size_t l, uint8_t f);
#endif
