#include <errno.h>
#include <fcntl.h>
#include <log.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <w64.h>

/*
*  We're writing however many bytes are supplied. Whatever it is should
*  be an divisible by 64 bits.  We don't check input.
*
*  We take a word, fill it with `1`, serialize.  Repeat alternately
*  with `0` until we've reached the size supplied.
*
*  To validate we do the exact opposite; checking the contents of the
*  word after unmarshalling, logging any differences we find.
*
*  Currently this is limited to the amount of RAM a box has because
*  the read/writes are not segmented.  That probably should get fixed
*  and some real error handling should be added at some point as well
*/

extern inline void pack(uint8_t *d, uint64_t s, size_t l);
inline void pack(uint8_t *d, uint64_t s, size_t l)
{
	/*
	* pack arbitrary bytes into a location
	*
	* accepts: destination, source, length in bytes
	*/
	do {
		(d)[--l] = s;
		s >>= 8;
	} while (l);
	return;
}

extern inline uint64_t unpack(uint8_t *s, size_t l);
inline uint64_t unpack(uint8_t *s, size_t l)
{
	/*
	* pull arbitrary bytes out of a given location and shove
	* in a 64 bit int.
	*
	* accepts: source, length in bytes
	*/
	uint64_t i = 0;
	do {
		l--;
		i += (uint64_t)s[l] << (l * 8);
	} while (l);
	return i;
}

bool check_byte(uint64_t d, uint8_t i)
{
	/*
	* iterate over a word, logging any bytes that don't match the
	* supplied pattern.
	*
	* accepts: word, pattern
	* returns: 0 if no bad bytes are found, 1 if any are
	*/
	uint8_t j = W_LEN;
	uint8_t byte;
	bool fail = 0;
	do {
		if ((byte = (uint8_t)(d >> --j * W_LEN)) != i) {
			printu(LOG_CRIT, "found mis-match on byte %u, %u != %u",
			       j, byte, i);
			fail = 1;
		}
	} while (j);
	return fail;
}

bool check_word(uint8_t *d, size_t l)
{
	/*
	* check that words in `d` are alternating between `~0x0` and `0`
	*
	* accepts: destination, length in bytes
	* returns: 0 if a word is matches, 1 on failure
	*/

	uint64_t i = MAX_INT;
	uint32_t j = l / 64;
	uint64_t word;
	uint64_t total_words = j;
	int o = -8;
	bool fail = 0;
	printu(LOG_DEBUG, "unmarshalling and validating data");
	do {
		if ((word = unpack(d + (o += W_LEN), W_LEN)) !=
		    (i = (i ? 0 : MAX_INT))) {
			fail = 1;
			printu(LOG_CRIT,
			       "found mis-match on word segment %u / %u!",
			       j / 64, total_words);
			check_byte(word, i);
			printb(word);
		}
	} while (j -= 8);
	return fail;
}

uint8_t *prep(size_t l)
{
	/*
	* malloc
	*/

	uint8_t *d;
	if ((d = malloc(l)) == NULL || errno)
		goto out;

	memset(d, '\0', l);
	if (errno)
		goto out;

	return d;
out:
	printu(LOG_CRIT, "%s: (%d) %s", __func__, errno, strerror(errno));
	exit(1);
}
void fill(uint8_t *d, size_t l)
{
	/*
	* fill a space, with alternating `~0x0` and `0` 64 bit words
	*
	* accepts: destination, length in bytes
	*/
	uint64_t i = MAX_INT;
	int o = -8;
	do {
		pack(d + (o += 8), (i = ((i) ? 0 : MAX_INT)), 8);
	} while (l -= 8);
	printu(LOG_DEBUG, "data serialized");
	return;
}

void input(uint8_t *d, const char *p, size_t l)
{
	/*
	* read from disk
	*
	* accepts: destination, path, size
	*/
	FILE *f;
	if (((f = fopen(p, "r")) == NULL) || errno)
		goto out;

	if ((fread(d, 1, l, f) < 0) || errno)
		if (ferror(f))
			goto out;

	if (fclose(f) || errno)
		goto out;

	printu(LOG_DEBUG, "read entire file");
	return;
out:
	printu(LOG_CRIT, "%s: (%d) %s", __func__, errno, strerror(errno));
	exit(1);
}

void output(uint8_t *d, const char *p, size_t l, bool f)
{
	/*
	* write to disk, async, only flusing if (flush) is defined
	* accepts: buffer, path, length in bytes
	*/
	size_t r;
	FILE *o;

	if (!access(p, F_OK)) {
		if (unlink(p) || errno)
			goto out;
	} else
		errno = 0;

	if (((o = fopen(p, "w")) == NULL) || errno)
		goto out;

	if (((r = fwrite(d, 1, l, o) < 0)) || errno)
		goto out;

	if (f) {
		if (fflush(o) || errno)
			goto out;
		sync();
	}

	if (fclose(o) || errno)
		goto out;

	printu(LOG_DEBUG, "wrote %u bytes", (r ? r : l));
	return;

out:
	printu(LOG_CRIT, "%s: (%d) %s", __func__, errno, strerror(errno));
	exit(1);
}

void printb(uint64_t i)
{
	/*
	 * print character representation of a 64 bit word
	 */
	char p[65];
	uint8_t j = sizeof(p) - 1;
	p[j] = '\0';
	while (j) {
		p[--j] = (i & 0x1) ? '1' : '0';
		i >>= 1;
	}
	printu(LOG_CRIT, "%s", p);
	fflush(stdout);
	return;
}

void run(uint8_t *d, const char *p, size_t l, uint8_t f)
{
	/*
	  * destination, path, length in bytes, whether or not we flush
	  *
	  */
	memset(d, '\0', l);
	fill(d, l);
	output(d, p, l, f);

	memset(d, '\0', l);
	input(d, p, l);

	if (check_word(d, l))
		exit(1);

	printu(LOG_DEBUG, "file contents are correct");
}
