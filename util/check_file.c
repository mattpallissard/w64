#include <w64.h>
#include <argp.h>
#include <log.h>
#include <stdlib.h>
#include <string.h>

const char *argp_program_version = "check_em v0.1";
const char *argp_program_bug_address = "Matt Pallissard <matpp@uw.edu>";
static char doc[] = "double check 'em";
static char args_doc[] = "";
static struct argp_option options[] = {
	{ "path", 'p', "*char", 0, "i/o path, should be a file" },
	{ "size", 's', "u64", 0, "size in gibibytes" },
	{ 0 }
};

struct arguments {
	char *path;
	uint16_t size;
};

static error_t parse_opt(int k, char *arg, struct argp_state *s)
{
	struct arguments *a = s->input;

	switch (k) {
	case 'p':
		a->path = arg;
		break;
	case 's':
		a->size = atoi(arg);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

size_t to_gib(uint32_t i)
{
	return i *= 1024 * 1024 * 1024;
}

int main(int argc, char **argv)
{
	struct arguments a;
	a.path = "I DONT EXIST LOL";
	a.size = -1;
	size_t s;
	uint8_t *d;
	setprintmask(LOG_DEBUG);
	setprintdate(1);
	argp_parse(&argp, argc, argv, 0, 0, &a);
	s = to_gib(a.size);
	d = prep(s);
	memset(d, '\0', s);
	input(d, a.path, s);
	if (check_word(d, s))
		exit(1);
	printu(LOG_INFO, "file contents are correct");
	exit(0);
}
