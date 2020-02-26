#include <assert.h>
#include <w64.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <log.h>

enum { D_SIZE = 1024 * 1024 * 1024 };

void f_test_pack()
{
	uint64_t u = 72340172838076673;
	uint8_t d[8];
	uint8_t i = 0;

	pack(d, u, 8);

	while (i < 8)
		assert(1 == d[i++]);
	return;
}

void f_test_unpack()
{
	uint8_t d[8];
	uint8_t i = 0;

	while (i < 8)
		d[i++] = 1;

	assert(unpack(d, 8) == 72340172838076673);
	return;
}

void f_test_fill()
{
	uint32_t j = D_SIZE;
	uint64_t o = -8;
	uint8_t *d = malloc(D_SIZE);
	memset(d, '\0', D_SIZE);
	uint64_t i = MAX_INT;
	fill(d, D_SIZE);

	do {
		assert(unpack(d + (o += W_LEN), W_LEN) ==
		       (i = (i ? 0 : MAX_INT)));
	} while (j -= 8);
	return;
}

void f_test_check_word()
{
	uint8_t *d = malloc(D_SIZE);
	memset(d, '\0', D_SIZE);
	fill(d, D_SIZE);
	assert(!check_word(d, D_SIZE));
	(d)[1] = 12;

	assert(check_word(d, D_SIZE));
	return;
}

void f_test_check_byte()
{
	uint64_t d = 0;
	assert(!check_byte(d, 0));

	assert(check_byte(d, ~0x0));
	d = ~0x0;
	assert(!check_byte(d, ~0x0));
	return;
}

int main(void)
{
	setprintmask(LOG_DEBUG);
	f_test_pack();
	f_test_unpack();
	f_test_fill();
	f_test_check_word();
}
